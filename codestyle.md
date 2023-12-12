#### The file contains code-style requirements on the project Argos.

##### Versions:
- C++ 17
- Qt 6.0

##### Files:

- Headers files:

   Header file should include declarations of functionality defined in one 
    concrete module \*.cpp file, all the functionality of the module should be easily described and solve one concrete problem.

  Preferably if header file will contain only one class's definition and name
of the class, otherwise name should represent what exactly module does.
Format of the header file must be \*.h.

Example of header filenames:
    
```
     some_class.h
     some_functionality.h
```

 - Source files:

   As a header files, source files should solve one concrete problem, preferably
    if it will contain definitions of method declared in one class.
    
Example of source filenames: 
    
```
     main.cpp
     some_class.cpp
     some_functionality.cpp
```

##### Header guards:
- For header guards should be used the "pragma once" directive.

    Example:
``` 
     #pragma once 
```

#####  Declarations and  definitions:
- Variable declarations:
    Variables should be declared on a separate lines, preferable if the variables will be declared as close as possible before the point at which they're used.
    In a pointer and reference declarations, specifier must be next from the type, separate from the name.
    
    Example:
``` 
     void* SomeFunc();  
     int& SomeMethod(); 
     double* somePointer;
```

##### Naming agreements:
- Names of the objects(variables), types(classes), and functions(method) must represent it's role in a project, describe functionality or contained value.

    Example
```
    int m_visitorsCounter;
    Status GetStatus();
    struct ClientData;
```

- For classes, methods and functions naming should be used PascalCase style.

    Example:
``` 
     void SomeFunc(); 
     class SomeClass; 
     int SomeMethod(); 
     ~SomeClass();
```

- For variables, constants and objects should be used camelCase style.
    For member variables of the class must be used m_ prefix.
    
    
    Example: 
```
     int someVar; 
     const int someConst; 
     classname someObj; 
     double m_memberVar;
```

##### Formatting:

- For include directive of libraries external to the project should be used angle brackets (<>), libraries current to the project should be enclosed in quote marks (""). 
    Order of includes - from most local to most generic.

    Example:
```
    #include "local_file.h" // header from this project

    #include <local_library/file.h> // other project library

    #include <external_lib> // external or standard library
```


- In a function or a method definition statements may be separated by meaning.

    Example:
```
    {
         SomeUserType obj(init);
         obj.SomeMethod();
         const type res = SomeRelatedFunc(obj);
    
         SomeOtherType var;
         SomeOtherFunc(var, res);
    
         return var;
     }
```


- Curly brackets always should be on a separate lines.


    Example: 
```  
   for(int i; i < n; i++) 
   {
       // some code 
   } 

   class ClassName  
   { 
       int var;
   public:
       someMethod();
   }
```  


- In a case of functions with multiple parameters, or if names are too long, they may be placed on separate lines


    Example: 
```  
   void SomeLongDeclaration(int someLongNamedParameter,
                            SomeClass anotherParameter,
                            const std::string oneMoreParameter);
```  


- Also, for better readability initialization lists must have multiple line formatting too


    Example: 
```  
   SomeClass::SomeClass(int someLongNamedParameter,
                        SomeOtherClass anotherParameter,
                        const std::string oneMoreParameter)
        : m_someLongNamedMember(someLongNamedParameter)
        , m_anotherMember(anotherParameter)
        , m_oneMoreMember(oneMoreParameter)
```  


##### Class definition:

Recommended order of class declaration:

- public types
- public methods
- public members
- protected types
- protected methods
- protected members
- private types
- private methods
- private members


    Example: 
```  
    class SomeClass
    {
    public:
        enum class PublicType;
        
        SomeClass();
        void PublicMethod();
        
        int publicMember;
        
    protected:
        enum class ProtectedType;
        
        void ProtectedMethod();
        
        int protectedMember;
        
    private:
        enum class PrivateType;
        
        void PrivateMethod();
        
        int privateMember;
    };
```  