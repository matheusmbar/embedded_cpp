#ifndef INCLUDE_TEST_CPP_H_
#define INCLUDE_TEST_CPP_H_

class Test_CPP {
 public:
  Test_CPP(int value);

  int getValue();
  void setValue(int value);

  int checkGlobals(void);

 private:
  int value_;
};

#endif
