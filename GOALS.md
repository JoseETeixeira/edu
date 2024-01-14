# GOALS

Package manager -> I want to be able to install C++ packages such as boost and others from a package manager that works like npm and converts those packages into my new language as it installs them

Pointer abstraction -> I want to pass all variables by reference as a default, if I want to pass by copy there should be a "copy" function (reserved keyword for that new language) that when called gets the variable by copy

Automated imports -> I want default C++ modules to be imported automatically when needed instead of importing each module manually, when importing non default modules, I want to import them like in typescript

Compilation -> When compiling the code to run, it should be transpiled into C++ then built as a regular C++ program

Classes, Interfaces and Types -> I want to be able to define classes, interfaces and types the same way that I do it in Typescript

Garbage Collection -> Garbage collection should be handled automatically by the language

Object creation -> I want to create new objects without the usage of the "new" keyword and invoking its class; I want to define an object like its done in typescript

Inheritance -> I want to use inheritance like it's done in typescript (e.g.: by using the "extends" keyword)

NULL -> I don't want to treat 0 as NULL, it should just be a null pointer, also the NULL keyword should be "null" instead.

Template Syntax -> I want to define templates as its done in typescript

printing -> I don't want to use cout and endline, instead I want to use print()

export keyword -> I want to be able to export types, functions, interfaces, etc to be used by other files, it should be used as "exports ..."

header files -> I dont want to use header files, everything should be able to be defined in the same file and imported as needed

memory allocation -> I want memory allocation to be handled automatically when creating a new object; Since we know the class of the object you should be able to do this

#define keyword -> I dont want to use #define, instead I want to treat a "const" as anything you would expect it to handle in a language like typescript

threads -> I want to use the "async/await" keywords to create threads, like it is handled in typescript

this keyword -> I dont want to use the "this" keyword anywhere.

auto keyword -> I dont want to explicitily use the auto keyword, since we know types beforehand, it should be automatical
