### Extended Backus-Naur Form (EBNF) Grammar

1. **Program Structure**

   ```ebnf
   program = { import_statement | class_declaration | interface_declaration | function_declaration | statement } ;

   import_statement = "import", identifier, "from", string_literal, ";" ;
   ```

2. **Class Declaration**

   ```ebnf
   class_declaration = "class", identifier, [ "extends", identifier ], "{", { class_member }, "}" ;

   class_member = constructor_declaration | method_declaration | property_declaration ;

   constructor_declaration = "constructor", "(", [ parameters ], ")", "{", { statement }, "}" ;

   method_declaration = "function", identifier, "(", [ parameters ], ")", "{", { statement }, "}" ;

   property_declaration = "let", identifier, [":", type], [ "=", expression ], ";" ;
   ```

3. **Interface Declaration**

   ```ebnf
   interface_declaration = "interface", identifier, "{", { interface_member }, "}" ;

   interface_member = property_declaration | method_signature ;

   method_signature = "function", identifier, "(", [ parameters ], ")", ";" ;
   ```

4. **Function Declaration**

   ```ebnf
   function_declaration = ["export"], "function", identifier, "(", [ parameters ], ")", "{", { statement }, "}" ;

   parameters = parameter, { ",", parameter } ;

   parameter = identifier, [ ":", type ] ;
   ```

5. **Statements and Expressions**

   ```ebnf
   statement = variable_declaration
             | expression_statement
             | return_statement
             | if_statement
             | for_statement
             | while_statement
             | async_statement
             | try_statement
             ;

   variable_declaration = "let", identifier, [":", type], [ "=", expression ], ";" ;

   expression_statement = expression, ";" ;

   return_statement = "return", [ expression ], ";" ;

   if_statement = "if", "(", expression, ")", "{", { statement }, "}", [ "else", "{", { statement }, "}" ] ;

   for_statement = "for", "(", [ expression ], ";", [ expression ], ";", [ expression ], ")", "{", { statement }, "}" ;

   while_statement = "while", "(", expression, ")", "{", { statement }, "}" ;

   async_statement = "async", function_declaration ;

   try_statement = "try", "{", { statement }, "}", "catch", "(", identifier, ")", "{", { statement }, "}" ;

   expression = assignment_expression | binary_expression | unary_expression | literal | identifier ;

   assignment_expression = identifier, "=", expression ;

   binary_expression = expression, binary_operator, expression ;

   unary_expression = unary_operator, expression ;

   binary_operator = "+" | "-" | "*" | "/" | "==" | "!=" | "<" | ">" | "<=" | ">=" ;

   unary_operator = "+" | "-" | "!" ;

   literal = numeric_literal | string_literal | "true" | "false" | "null" ;

   numeric_literal = digit, { digit } ;

   string_literal = '"', { char }, '"' | "'", { char }, "'" ;

   digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

   char = ? any character except quotes ? ;

   identifier = letter, { letter | digit | "_" } ;

   letter = "a" | "b" | "c" | "d" | ... | "z" | "A" | "B" | "C" | "D" | ... | "Z" ;

   type = identifier | array_type | function_type ;

   array_type = identifier, "[", "]" ;

   function_type = "(", [ parameters ], ")", "=>", type ;
   ```

6. **Garbage Collection and Memory Management**

   - These features do not have direct syntactic representations in EBNF as they pertain to the language's runtime behavior and memory model.

7. **Pointer Abstraction and `copy` Function**

   ```ebnf
   copy_function = "copy", "(", identifier, ")" ;
   ```

8. **Object Creation**

   ```ebnf
   object_creation = identifier, "(", [ arguments ], ")" ;

   arguments = expression, { ",", expression } ;
   ```

9. **Template Syntax**

   ```ebnf
   template_declaration = "template", "<", identifier, ">", "{", { template_member }, "}" ;

   template_member = class_declaration | function_declaration | interface_declaration ;

   ```

10. **Printing**

```ebnf
print_statement = "console.log", "(", [ expression ], ")", ";" ;
```

11. **Async/Await**

```ebnf
async_function_declaration = "async", function_declaration ;

await_expression = "await", expression ;
```
