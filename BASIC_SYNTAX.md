## BASIC SYNTAX

### 1. Types

Boolean: bool
Character: char
Integer: int
Floating point: float
Valueless: void
Wide character: wchar_t
String: string
Error: Error

the Error type is composed of the message and the errorCode, where both are of type string

### 2. Variable Declaration

```
Type variableName = value; // Mutable variable where Type is any C++ type (int, float, etc.)
const Type CONSTANT_NAME = value; // Constant
```

### 3. Functions

The `export` keyword is used to make functions available for import. Type is any C++ type (int, float, etc.).

```
export Type function functionName(Type parameter1, Type parameter2) {
  // Function body
  print("Function called");
}
```

### 4. Classes and Inheritance

The arguments are optional (and can be as many as the developer wants) and the Base Class can be constructed with or without them. Type is any C++ type (int, float, etc.).

```
class BaseClass {
  constructor(Type argument1, Type argument2) {
    // Constructor code
  }

  Type method() {
    // Method implementation
  }
}

class DerivedClass extends BaseClass {
  constructor(Type argument1, Type argument2) {
    super(); // Calling the base class constructor
  }

  Type anotherMethod() {
    // Another method
  }
}
```

### 5. Interfaces

Interfaces are used to define contracts for classes.

```
interface MyInterface {
  Type property;
  ReturnType method(Type parameter);
}
```

### 6. Object Creation

Objects are created directly, without using the `new` keyword.

```
DerivedClass object = DerivedClass(argument1, argument2);
```

### 7. Templates and Generics

```
T function genericFunction<T>(arg: T) {
  return arg;
}
```

### 7. Async/Await for Threading

Async functions and await are used for handling asynchronous operations.

```
async Type function asyncFunction() {
  AsyncFunctionType result = await anotherAsyncFunction();
  print(result);
}
```

### 8. Error Handling

Error handling can use try-catch blocks, similar to TypeScript.

```
try {
  // Code that might throw an error
} catch (Error error) {
  console.error(error);
}
```

### 9. Modules and Imports

Modules are imported in a manner similar to TypeScript.

```
import { functionName, ClassName } from "module-name";
```

### 10. Memory Allocation and Garbage Collection

These are handled automatically by the language runtime, so they do not have direct syntactic representations.

### 11. Pointers and References

All variables are passed by reference by default. To pass by copy, a `copy` function is used.

```
Type function takesCopy(value) {
  let copyValue = copy(value);
  // Use copyValue
}
```

### 12. NULL Handling

`null` is used instead of `NULL` or `0` for null references.

```
Type nullableVariable = null;
```

### 13. Printing to Console

The `print()` function is used for printing.

```
print("This is a message");
```

### 14. Receiving input

The `input()` function is used for receiving inputs from the user.

```
Type data = input(); //populates data with the value received from the input function cast to Type
```
