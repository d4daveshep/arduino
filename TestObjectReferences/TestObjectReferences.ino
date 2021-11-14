#line 2 "sketch_nov14a"

#include <AUnit.h>

class Child {
  private:
  int anInt = 345;

  public:
  int getInt() {
    return this->anInt;
  }
  void setInt(int i) {
    this->anInt = i;
  }
};

class Parent {
  private:
  Child &child;

  public:
  Parent(Child& theChild) {
    this->child = theChild;
  }
  void setChild(Child& newChild) {
    this->child = newChild;
  }
  Child& getChild() {
    return this->child;
  }
  int getChildInt() {
    return child.getInt();
  }
  void setChildInt(int i) {
    getChild().setInt(i);
  }
  
};

//Child myChildObject;
//Parent myParentObject;//(myChildObject);

test(childIntReference) {
  Child aChild;
  assertEqual(345,aChild.getInt());

  aChild.setInt(234);
  assertEqual(234,aChild.getInt());

  Child &bChild = aChild;
  assertEqual(234,bChild.getInt());

  bChild.setInt(123);
  assertEqual(123,aChild.getInt());
  assertEqual(123,bChild.getInt());
  
}

test(passByReference) {

  Child aChild; // create child object - this is the only child object created
  assertEqual(345,aChild.getInt());
  
  aChild.setInt(123); // change the internal value
  Child& bChild = aChild; // create a refence to the child object
  assertEqual(123,bChild.getInt());
  
  Parent aParent(bChild); // create a parent object with a reference to the child
  assertEqual(123,aParent.getChildInt());
  assertEqual(123,aParent.getChild().getInt());

  aParent.setChildInt(789); // update the child int via the parent
  assertEqual(789,aParent.getChildInt());


}

void setup() {
  // put your setup code here, to run once:
  delay(1000); // wait for stability on some boards to prevent garbage Serial
  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  while(!Serial); // for the Arduino Leonardo/Micro only
}

void loop() {
  // put your main code here, to run repeatedly:
  aunit::TestRunner::run();
}
