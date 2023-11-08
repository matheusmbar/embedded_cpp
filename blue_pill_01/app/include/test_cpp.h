#ifndef APP_INCLUDE_TEST_CPP_H_
#define APP_INCLUDE_TEST_CPP_H_

class Test_CPP {
 public:
  explicit Test_CPP(int value);

  int getValue();
  void setValue(int value);

  int checkGlobals(void);

 private:
  int value_;
};

#endif  // APP_INCLUDE_TEST_CPP_H_
