#include <AndroidClient.h>
#include <jni.h>
#include <android/log.h>

class AndroidClient : public HTTPClient {
 public:

	AndroidClient(JNIEnv * _env, const std::string & _user_agent, bool _enable_cookies, bool _enable_keepalive)
 : HTTPClient(_user_agent, _enable_cookies, _enable_keepalive), env(_env) {


		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidClient Constructor called");

	}

	void androidInit(){

		cookieManagerClass =  env->FindClass("android/webkit/CookieManager");
		httpClass = env->FindClass("java/net/HttpURLConnection");
		urlClass = env->FindClass("java/net/URL");
	 	bufferedReaderClass = env->FindClass("java/io/BufferedReader");
	 	inputStreamReaderClass = env->FindClass("java/io/InputStreamReader");
	 	inputStreamClass = env->FindClass("java/io/InputStream");



		//getInputStreamMethod = env->GetMethodID(env->FindClass("java/net/URLConnection"), "getInputStream", "()Ljava/io/InputStream;");
	 	readerCloseMethod = env->GetMethodID(bufferedReaderClass, "close", "()V");
	 	readLineMethod = env->GetMethodID(bufferedReaderClass, "readLine", "()Ljava/lang/String;");
	 	readMethod = env->GetMethodID(inputStreamClass, "read", "()I");
	 	inputStreamReaderConstructor = env->GetMethodID(inputStreamReaderClass, "<init>", "(Ljava/io/InputStream;)V");
	 	bufferedReaderConstructor = env->GetMethodID(bufferedReaderClass, "<init>", "(Ljava/io/Reader;)V");
		urlConstructor =  env->GetMethodID(urlClass, "<init>", "(Ljava/lang/String;)V");
		openConnectionMethod = env->GetMethodID(urlClass, "openConnection", "()Ljava/net/URLConnection;");
		setRequestProperty = env->GetMethodID(httpClass, "setRequestProperty", "(Ljava/lang/String;Ljava/lang/String;)V");
		setRequestMethod = env->GetMethodID(httpClass, "setRequestMethod", "(Ljava/lang/String;)V");
		setDoInputMethod = env->GetMethodID(httpClass, "setDoInput", "(Z)V");
		connectMethod = env->GetMethodID(httpClass, "connect", "()V");
		getResponseCodeMethod = env->GetMethodID(httpClass, "getResponseCode", "()I");
		getResponseMessageMethod = env->GetMethodID(httpClass, "getResponseMessage", "()Ljava/lang/String;");
		setRequestPropertyMethod =  env->GetMethodID(httpClass, "setRequestProperty", "(Ljava/lang/String;Ljava/lang/String;)V");
		clearCookiesMethod =  env->GetMethodID(cookieManagerClass, "removeAllCookie", "()V");
		getInputStreamMethod =  env->GetMethodID(httpClass, "getInputStream", "()Ljava/io/InputStream;");
		inputStreamCloseMethod = env->GetMethodID(inputStreamClass, "close", "()V");

		initDone = true;

	}

  HTTPResponse request(const HTTPRequest & req, const Authorization & auth){


		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidClient request called");

  	if (!initDone){
  		androidInit();
  	}


  	jobject url = env->NewObject(urlClass, urlConstructor, env->NewStringUTF(req.getURI().c_str()));
  	//jobject url = env->NewObject(urlClass, urlConstructor, "sometrik.com");
  	jobject connection = env->CallObjectMethod(url, openConnectionMethod);


  	//Authorization example
		//env->CallVoidMethod(connection, setRequestPropertyMethod, env->NewStringUTF("Authorization"), env->NewStringUTF("myUsername"));

		//  std::string auth_header = auth.createHeader();

		// if (!auth_header.empty()) {
		//		env->CallVoidMethod(connection, setRequestPropertyMethod, env->NewStringUTF(auth.getHeaderName()), env->NewStringUTF(auth_header.c_str()));
		//}


		//Set Follow enabled
		switch (req.getType()) {
		case HTTPRequest::POST:
		  	env->CallVoidMethod(connection, setRequestMethod, env->NewStringUTF("POST"));
			break;
		case HTTPRequest::GET:
				 env->CallVoidMethod(connection, setRequestMethod, env->NewStringUTF("GET"));;
			break;
		}


		//Brings out exception, if URL is bad --- needs exception (IOExcpetion) handling or something
			int responseCode = env->CallIntMethod(connection, getResponseCodeMethod);

			if (env->ExceptionCheck()) {
			   env->ExceptionClear();
				__android_log_print(ANDROID_LOG_INFO, "AndroidClient", "EXCPETION http request responsecode = %i", responseCode);
			  return HTTPResponse(0, "exception");
			}
			__android_log_print(ANDROID_LOG_INFO, "AndroidClient", "http request responsecode = %i", responseCode);


			const char *errorMessage = "";

		if (responseCode >= 400 && responseCode <= 599){
			jstring javaMessage = (jstring)env->CallObjectMethod(connection, getResponseMessageMethod);
			errorMessage = env->GetStringUTFChars(javaMessage, 0);

		}

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "going to else");
		// stream
		if (callback) {
			while ( 1 ) {
				  // lue streamia
					callback->handleChunk(0, 0);
			}
  		return HTTPResponse(responseCode, errorMessage);
		} else {
			// lue koko stream

			jobject input = env->CallObjectMethod(connection, getInputStreamMethod);
			env->ExceptionClear();

			jmethodID blaah = env->GetMethodID(inputStreamClass, "read", "([B)I");
			jbyteArray array = env->NewByteArray(4096);
			int g = 0;
			std::string content;
			__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "going in while");


			while ((g = env->CallIntMethod(input, blaah, array)) != -1) {
				__android_log_print(ANDROID_LOG_INFO, "AndroidClient", " stream =  %i", g);

				jbyte* content_array = env->GetByteArrayElements(array, NULL);
				content += std::string((char*)content_array, g);

				env->ReleaseByteArrayElements(array, content_array, JNI_ABORT);

			}

			__android_log_print(ANDROID_LOG_INFO, "content", "contentti = %s", content.c_str());

			//env->CallVoidMethod(input, inputStreamCloseMethod);


			return HTTPResponse(responseCode, errorMessage, "", content);
		}
  }

  void clearCookies() {

  	env->CallVoidMethod(cookieManagerClass, clearCookiesMethod);

  }

 protected:
  bool initialize() { return true; }


 private:
	bool initDone = false;

  JNIEnv * env;
  jclass cookieManagerClass;
  jmethodID clearCookiesMethod;

  jclass bitmapClass;
  jclass factoryClass;
  jclass httpClass;
  jclass urlClass;
  jclass bufferedReaderClass;
  jclass inputStreamReaderClass;
  jclass inputStreamClass;
  jmethodID urlConstructor;
  jmethodID openConnectionMethod;
  jmethodID setRequestProperty;
  jmethodID setRequestMethod;
  jmethodID setDoInputMethod;
  jmethodID connectMethod;
  jmethodID getResponseCodeMethod;
  jmethodID getResponseMessageMethod;
  jmethodID setRequestPropertyMethod;
  jmethodID outputStreamConstructor;
  jmethodID factoryDecodeMethod;
  jmethodID getInputStreamMethod;
  jmethodID bufferedReaderConstructor;
  jmethodID inputStreamReaderConstructor;
  jmethodID readLineMethod;
  jmethodID readerCloseMethod;
  jmethodID readMethod;
  jmethodID inputStreamCloseMethod;

};

std::shared_ptr<HTTPClient>
AndroidClientFactory::createClient(const std::string & _user_agent, bool _enable_cookies, bool _enable_keepalive) {
  return std::make_shared<AndroidClient>(env, _user_agent, _enable_cookies, _enable_keepalive);
}

