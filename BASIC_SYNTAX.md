## BASIC SYNTAX

### 1. Variable Declaration

```typescript
let variableName = value; // Mutable variable
const CONSTANT_NAME = value; // Constant
```

### 2. Functions

Functions are defined similarly to TypeScript. The `export` keyword is used to make functions available for import.

```typescript
export function functionName(parameter1, parameter2) {
  // Function body
  console.log("Function called");
}
```

### 3. Classes and Inheritance

Classes and inheritance follow a TypeScript-like syntax.

```typescript
class BaseClass {
  constructor() {
    // Constructor code
  }

  method() {
    // Method implementation
  }
}

class DerivedClass extends BaseClass {
  constructor() {
    super(); // Calling the base class constructor
  }

  anotherMethod() {
    // Another method
  }
}
```

### 4. Interfaces

Interfaces are used to define contracts for classes.

```typescript
interface MyInterface {
  property: Type;
  method(parameter: Type): ReturnType;
}
```

### 5. Object Creation

Objects are created directly, without using the `new` keyword.

```typescript
let object = DerivedClass();
```

### 6. Templates and Generics

Template syntax is inspired by TypeScript.

```typescript
function genericFunction<T>(arg: T): T {
  return arg;
}
```

### 7. Async/Await for Threading

Async functions and await are used for handling asynchronous operations.

```typescript
async function asyncFunction() {
  let result = await anotherAsyncFunction();
  console.log(result);
}
```

### 8. Error Handling

Error handling can use try-catch blocks, similar to TypeScript.

```typescript
try {
  // Code that might throw an error
} catch (error) {
  console.error(error);
}
```

### 9. Modules and Imports

Modules are imported in a manner similar to TypeScript.

```typescript
import { functionName, ClassName } from "module-name";
```

### 10. Memory Allocation and Garbage Collection

These are handled automatically by the language runtime, so they do not have direct syntactic representations.

### 11. Pointers and References

All variables are passed by reference by default. To pass by copy, a `copy` function is used.

```typescript
function takesCopy(value) {
  let copyValue = copy(value);
  // Use copyValue
}
```

### 12. NULL Handling

`null` is used instead of `NULL` or `0` for null references.

```typescript
let nullableVariable = null;
```

### 13. Printing to Console

The `console.log()` function is used for printing.

```typescript
console.log("This is a message");
```
