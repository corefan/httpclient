#ifndef _AUTHORIZATION_H_
#define _AUTHORIZATION_H_

#include <string>

class Authorization {
 public:
  Authorization() { }
  virtual ~Authorization() { }
  virtual const char * getHeaderName() const { return "Authorization"; }
  virtual std::string createHeader() const { return ""; }

  static Authorization noAuth;
};

#endif
