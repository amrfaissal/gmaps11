#include "http/http_client.hpp"

using namespace http;
using namespace meta;
namespace py = boost::python;


Client::Client(std::string key, std::string client_id,
		std::string client_secret, int timeout, int connect_timeout, int retry_timeout) {

	if (key.empty() && (client_id.empty() && client_secret.empty())) {
		throw std::domain_error(
				"Must provide API key or enterprise credentials when creating client.");
	}

	if (!key.empty() && !boost::starts_with(key, "AIza")) {
		throw std::domain_error("Invalid API key provided.");
	}

	this->key_ = key;

	if (timeout > 0 && connect_timeout > 0) {
		throw std::domain_error("Specify either timeout, or connect_timeout.");
	}

	if (connect_timeout > 0) {
		this->connect_timeout_ = connect_timeout;
	} else {
		this->timeout_ = timeout;
	}

	this->client_id_ = client_id;
	this->client_secret_ = client_secret;
	this->retry_timeout_ = retry_timeout;

	try
	{
		Py_Initialize();
		// We can insert the module working directory into the Python path
		// so module search can take advantage, but the module is already
		// installed using CMake.
		this->crypto_mod_ = py::import("crypto");
	}
	catch (boost::python::error_already_set const &)
	{
		std::string perror_str = parse_python_exception();
		std::cerr << perror_str << std::endl;
	}
}

std::string Client::urlencode_params(const UrlParams & params) {
	std::stringstream ss;
	std::vector<std::string> _out_;
	for (const std::pair<std::string,std::string>& params_pair: params) {
		ss << params_pair.first << "=" << params_pair.second;
		_out_.push_back(ss.str());
		ss.str("");
	}
	return boost::algorithm::join(_out_, "&");
}

std::string Client::sign_url(std::string secret, std::string url_payload) {
	const char * _sig_ = nullptr;
	try
	{
		py::object _sigobj_ = crypto_mod_.attr("url_signer")(url_payload, secret);
		_sig_ = py::extract<const char*>(py::str(_sigobj_));
	}
	catch (boost::python::error_already_set const &) {
		std::string perror_str = parse_python_exception();
		std::cerr << perror_str << std::endl;
	}
	return std::string(_sig_);
}

std::string Client::generate_auth_url(std::string path, UrlParams params,
		bool accepts_clientid) {
	std::stringstream ss;

	if (accepts_clientid && !client_id_.empty() && !client_secret_.empty()) {
		params["client"] = client_id_;

		// Build URL with encoded parameters
		ss << path << "?" << urlencode_params(params);
		std::string urlToSign = ss.str();

		// Sign the builded URL
		std::string signature = sign_url(client_secret_, urlToSign);
		ss.str(""); // Reset string stream
		ss << googleapis_BASE_URL << urlToSign << "&signature=" << signature;

		return ss.str();
	}

	if (!key_.empty()) {
		params["key"] = key_;
		ss << googleapis_BASE_URL << path << "?" << urlencode_params(params);
		return ss.str();
	}
	throw std::domain_error("Must provide API key for this API. "
			"It does not accept enterprise credentials.");
}

std::string Client::get_body(std::string response) {
	std::string err;
	json11::Json json_body = json11::Json::parse(response, err);
	if (!err.empty())
		throw std::domain_error("Bad JSON while parsing body: " + err);

	std::string api_status = json_body["status"].string_value();
	if (api_status == "OK" || api_status == "ZERO_RESULTS") {
		return json_body.dump();
	}

	if (api_status == "OVER_QUERY_LIMIT") {
		throw googlemaps::exceptions::retriable_request_error();
	}

	// Check if there is an error message
	std::string error_msg = json_body["error_message"].string_value();
	if (!error_msg.empty()) {
		throw googlemaps::exceptions::api_error(api_status, error_msg);
	} else {
		throw googlemaps::exceptions::api_error(api_status);
	}
}

double Client::unirand() {
	// seed with the system clock
	srand(time(0));
	return rand() / double(RAND_MAX);
}

std::string Client::parse_python_exception() {
	// Parses the value of the active python exception
	// NOTE SHOULD NOT BE CALLED IF NO EXCEPTION
	PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
	// Fetch the exception info from the Python C API
	PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

	// Fallback error
	std::string ret("Unfetchable Python error");
	// If the fetch got a type pointer, parse the type into the exception string
	if (type_ptr != NULL) {
		py::handle<PyObject> h_type(type_ptr);
		py::str type_pstr(h_type);
		// Extract the string from the boost::python object
		py::extract<std::string> e_type_pstr(type_pstr);
		// If a valid string extraction is available, use it
		//  otherwise use fallback
		if (e_type_pstr.check())
			ret = e_type_pstr();
		else
			ret = "Unknown exception type";
	}
	// Do the same for the exception value (the stringification of the exception)
	if (value_ptr != NULL) {
		py::handle<PyObject> h_val(value_ptr);
		py::str a(h_val);
		py::extract<std::string> returned(a);
		if (returned.check())
			ret += ": " + returned();
		else
			ret += std::string(": Unparseable Python error: ");
	}
	// Parse lines from the traceback using the Python traceback module
	if (traceback_ptr != NULL) {
		py::handle<PyObject> h_tb(traceback_ptr);
		// Load the traceback module and the format_tb function
		py::object tb(py::import("traceback"));
		py::object fmt_tb(tb.attr("format_tb"));
		// Call format_tb to get a list of traceback strings
		py::object tb_list(fmt_tb(h_tb));
		// Join the traceback strings into a single string
		py::object tb_str(py::str("\n").join(tb_list));
		// Extract the string, check the extraction, and fallback in necessary
		py::extract<std::string> returned(tb_str);
		if (returned.check())
			ret += ": " + returned();
		else
			ret += std::string(": Unparseable Python traceback");
	}
	return ret;
}

std::string Client::Get(std::string url, UrlParams params,
		bool accepts_clientid, boost::optional<Timestamp> first_request_time,
		int retry_counter, std::string base_url,
		std::function<std::string(std::string)> extract_body) {

	if (!first_request_time) {
		first_request_time = system_clock::now();
	}

	duration<double> elapsed_time = system_clock::now() - first_request_time.get();
	if (elapsed_time.count() > retry_timeout_) {
		throw googlemaps::exceptions::timeout_error();
	}

	if (retry_counter > 0) {
		// 0.5 * (1.5 ^ i) is an increased sleep time of 1.5x per iteration,
		// starting at 0.5s when retry_counter=0. The first retry will occur
		// at 1, so subtract that first.
		double delay_seconds = 0.5
				* std::pow(1.5, (double) (retry_counter - 1));
		// Jitter this value by 50% and pause
		std::this_thread::sleep_for(
				seconds(
						boost::lexical_cast<int64_t>(
								delay_seconds * (unirand() + 0.5))));
	}

	std::string authed_url = generate_auth_url(url, params, accepts_clientid);

	std::stringstream response_stream;
	// Create writer to handle the stream
	curl::curl_writer owriter(response_stream);
	// Content will be written to writer
	curl::curl_easy easy(owriter);
	easy.add(curl_pair<CURLoption, string>(CURLOPT_URL, authed_url));
	easy.add(curl_pair<CURLoption, long>(CURLOPT_FOLLOWLOCATION, 1L));
	// CURL timeout defaults to 0 -> which means never times out.
	easy.add(curl_pair<CURLoption, long>(CURLOPT_TIMEOUT, (timeout_ < 0) ? 0 : timeout_));
	// CURL connect timeout -> Set to 0 to switch to defaut built-in value (300 seconds)
	easy.add(curl_pair<CURLoption, long>(CURLOPT_CONNECTTIMEOUT, (connect_timeout_ < 0) ? 0 : connect_timeout_));

	try {
		// Perform the GET request
		easy.perform();
		// Check if response code is in RETRIABLE_STATUSES
		auto reqhttpCode = easy.get_info<long>(CURLINFO_RESPONSE_CODE);
		if (in<long>(*reqhttpCode, RETRIABLE_STATUSES)) {
			// Retry request
			return this->Get(url, params, accepts_clientid, first_request_time, retry_counter + 1,
					base_url, extract_body);
		}

		// Extract the body of the response
		if (extract_body != nullptr) {
			return extract_body(response_stream.str());
		}
		return get_body(response_stream.str());
	}
	catch (googlemaps::exceptions::retriable_request_error & err) {
		return this->Get(url, params, accepts_clientid, first_request_time, retry_counter + 1,
				base_url, extract_body);
	}
	catch(googlemaps::exceptions::base_error& err) {
		// Catch other errors and log them for now
		std::cerr << err.__str__() << std::endl;
	}
	catch (curl_easy_exception & error) {
		error.print_traceback();
	}
	return std::string("");
}
