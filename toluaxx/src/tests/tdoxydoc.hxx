
//tolua_begin
/**
   \~english Documentation About TestDoc class
   \~russian Документация по классу TestDoc
**/
class TestDoc{
public:
  int var1; //*< \~english var1 in C++-style doc \~russian Документация по var1 в стиле C++

  double var2; /**< \~english var2 in C-style doc \~russian Документация по var2 в стиле C */
  void func1(char arg1,float arg2){
    
  }
private:
  int var3;
public:

  TestDoc(){
  }
protected:
  char var4;
  
};

/**
   \~english Not doc this
   \~russian Не документировать это
**/
class TestNotDoc{
  
};
//tolua_end
