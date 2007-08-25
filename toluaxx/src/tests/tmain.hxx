
//tolua_begin
class AutoGenTest{
private:
  int a[8];
public:
};

unsigned int AutoGenSizeOf(AutoGenTest*o=NULL){
  if(o)return sizeof(*o);
  return sizeof(AutoGenTest);
}
//tolua_end
