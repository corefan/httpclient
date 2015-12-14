#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include "Authorization.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPClientInterface.h"

#include <map>
#include <string>

class HTTPClient {
 public:
  HTTPClient(const std::string & _interface, const std::string & _user_agent, bool _enable_cookies, bool _enable_keepalive)
    : interface_name(_interface),
    user_agent(_user_agent),
    enable_cookies(_enable_cookies),
    enable_keepalive(_enable_keepalive)
    { }  
  HTTPClient(const HTTPClient & other)
    : interface_name(other.interface_name),
    user_agent(other.user_agent),
    cookie_jar(other.cookie_jar),
    enable_cookies(other.enable_cookies),
    enable_keepalive(other.enable_keepalive)
      {
      }
  virtual ~HTTPClient() { }
  
  HTTPClient & operator=(const HTTPClient & other) {
    if (this != &other) {
      interface_name = other.interface_name;
      user_agent = other.user_agent;
      cookie_jar = other.cookie_jar;
      enable_cookies = other.enable_cookies;
      enable_keepalive = other.enable_keepalive;
    }
    return *this;
  }
  
  HTTPResponse Post(const std::string & uri, const std::string & data) {
    HTTPRequest req(HTTPRequest::POST, uri);
    req.setContent(data);
    req.setContentType("application/x-www-form-urlencoded;charset=UTF-8");
    return request(req, Authorization::noAuth);
  }

  HTTPResponse Post(const std::string & uri, const std::string & data, const Authorization & auth) {
    HTTPRequest req(HTTPRequest::POST, uri);
    req.setContent(data);
    req.setContentType("application/x-www-form-urlencoded;charset=UTF-8");
    return request(req, auth);
  }
  
  HTTPResponse Get(const std::string & uri, const std::string & data, const Authorization & auth, bool follow_location = true, int timeout = 0) {
    std::string uri2 = uri;
    uri2 += '?';
    uri2 += data;
    HTTPRequest req(HTTPRequest::GET, uri2);
    req.setFollowLocation(follow_location);
    req.setTimeout(timeout);
    return request(req, auth);
  }
  
  HTTPResponse Get(const std::string & uri, bool follow_location = true, int timeout = 0) {
    HTTPRequest req(HTTPRequest::GET, uri);
    req.setFollowLocation(follow_location);
    req.setTimeout(timeout);
    return request(req, Authorization::noAuth);
  }
  
  HTTPResponse Get(const std::string & uri, const Authorization & auth, bool follow_location = true, int timeout = 0) {
    HTTPRequest req(HTTPRequest::GET, uri);
    req.setFollowLocation(follow_location);
    req.setTimeout(timeout);
    return request(req, auth);
  }

  void setCookieJar(const std::string & filename) { cookie_jar = filename; }
  void setCallback(HTTPClientInterface * _callback) { callback = _callback; }

  virtual HTTPResponse request(const HTTPRequest & req, const Authorization & auth) = 0;
  virtual void clearCookies() = 0;
        
 protected:
  virtual bool initialize() { return true; }

  std::string data_out, data_in;
  HTTPClientInterface * callback = 0;
  std::string interface_name;
  std::string user_agent;
  std::string cookie_jar;
  bool enable_cookies, enable_keepalive;
};

#endif
