
2006-10-25:
	Добавил директиву tolua_index, позволяющую специфицировать
	методы доступа .set{i,s}.
	Синтаксис: value_type tolua_index(key_type,get_method,set_method);
	В качестве образца ее работы продемонстрируем следующее:
	
	-------tolua-index-test.hxx----------
	class TOBJ{
	....
	};
	....
	class TCLASS{
	....
	  TOBJ* get(string);
	  void set(string,TOBJ*);
	  /**TOBJ* tolua_index(string,get,set);**/
	....
	};
	
	-------tolua-index-test.lua----------
	local g1=TCLASS()
	g1.o1=OBJ()    --- срабатывает set метод
	local o1=g1.o1 --- вызывается get метод
	

2006-10-24:
	Чтобы garbage collector не удалял userdata, добавленные
	в объект методами .set{i,s}, организовал их внесение в
	peer.
	
	Суть решенной проблемы:
	
	local g1=GRP()
	local o1=OBJ()
	
	g1.o1=o1
	collectgarbage"collect" --- объект o1 не удаляется,
				--- ссылка еще существует
				--- в локальной среде
	
	g1.o2=OBJ()
	collectgarbage"collect" --- объект g1.o2 удалится
				--- он был создан в луа и
				--- ссылки на него нигде нет
	--- этот фикс решает проблему (ссылка на объект помещается
	--- в peer g1, не давая garbage collector удалить его)


2006-10-04:
	Добавил спецификатор значения по умолчанию asnil. Теперь
	специфицированные так переменные будут интерпретированы
	как nil, если они равны значению по умолчанию.

	Например:
	------asnil-test.hxx----------------
	class TCLASS{
	       ....
	 public:
	  string menu;
	  void item(string&name){
	    int p,s;
	    if((p=menu.find(name+":"))<menu.length()){
	      p=p+name.length()+1;
	      s=menu.find(" ",p);
	      name=menu.substr(p,s-p);
	    }else name="";
	  }
	};
	------asnil-test.pxx----------------
	class TCLASS{
	       ....
	 public:
	  string menu;
	  void item(string&name="" asnil);
	};
	------asnil-test.lua----------------
	require"asnil-test"
	local t=TCLASS()
	t.menu="first:call second:of third:cthulhu"
	assert(t:item("first")=="call")
	assert(t:item("second")=="of")
	assert(t:item("third")=="cthulhu")
	assert(t:item("fourth"==nil)

	Это весьма полезно использовать в итераторе (операция `()`)
	(пример в src/tests/*index.*)
	

	Также исправлен баг со вставкой имени включаемого файла #include"".
	

2006-10-03:
	Добавил макрос tolua_callmethod(class_name,method_name,arguments,
	num_of_returns,returns), позволяющий вызывать метод класса, в 
	качестве которого может выступать функция объявленная в lua.
	В качестве класса используется `this`.

	Например:
	------call-method-test.hxx----------
	class TCLASS{
	 public:
	  float delta;
	  float time;
	       ....
	  void make(float period){
	    tolua_callmethod(TCLASS,handler,
	      tolua_pushnumber(L,time);
	      tolua_pushnumber(L,period),
	      1,
	      time=tolua_tonumber(L,1)
	    );
	  }
	};
	------call-method-test.lua----------
	require"call-method-test"
	local t=TCLASS()
	function t:handler(time,period)
	   time=time+period
	   self.delta=self.delta+period
	   return time
	end

	t:make(10)
	

2006-09-02:
	Исправлен баг с поиском включаемых методами ${i,l,h,c}file"" файлов.
	Теперь файлы ищутся сначала в директории, откуда запущен препроцессор,
	затем в директории, где расположен обрабатываемый файл.

	
2006-09-29:
	Теперь индексация (оператор `[]`) работает и для строк в качестве
	ключей. Используется метод `__{index,newindex}\\.{get,set}s`.

	Исправлен баг с обявлением переменных в namespace-ах, теперь все
	работает. (!Пока поддерживаются невложенные namespace!)
	
	
2006-09-28:
	Добавил оператор:
	`()` (__call\\.callself) - вызов себя. Его планируется использовать
	в качестве итератора. (пример в src/tests/*index.*)

	
2006-09-27:
	Добавил возможность объявления унарных операторов, таких как:
	`-` (__unm\\.unm) - унарный минус (C++ эквивалент `-`)
	`#` (__len\\.len) - оператор длины (в качестве эквивалента в 
	                    C++ принят `~`)

2006-09-26:
	Создана универсальная кроссплатформенная система сборки на базе Makefile,
	интерпретируемых утилитой GNU Make.
	Для требуемой ОС платформы и соллекции компиляторов создается свой
	конфигурационный файл `config.$(PLATFORM)-$(COMPILER)` и пользовательский
	конфигурационный файл `config.$(PLATFORM)-$(COMPILER).local`.
	В файле `config` указываются переменные конфигурации сборщика, такие как:
	  MODE=test|real            # режим работы сборщика
	  DEBUG=enable|disable      # режим отладки
	  PATH=/usr|....            # путь установки
	  PLATFORM=lunix|win32|.... # платформа
	  COMPILER=gcc|msvs|....    # компилятор
	  .....
	(подробности в руководстве по сборке и в комментариях `config` файла.)

	Сборщик анализирует конфигурационные файлы модулей, находящиеся в
	поддиректориях `src`, и на основе этих данных генерирует зависимости,
	цели сборки и их команды.
	
	Cборка производится командой:
	[shell:]# make all
	установка:
	[shell:]# make install
	удаление:
	[shell:]# make uninstall
	
	
2006-09-25:
	Добавил операторы lua5.1, такие как:
	`^`  (__pow\\.pow)       - возведение в степень (C++ эквивалент `^`)
	`..` (__concat\\.concat) - склеивание (в C++ в качестве эквивалента
	                           взята операция поразрядного или `|`)
	``

2006-09-21:
	Полностью переписал C код src/bin на C++. Теперь последовательность
	аргументов совершенно произвольная.
