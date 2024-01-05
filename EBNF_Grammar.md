### EBNF Grammar for Your Language

#### 1. Lexical Tokens

```
digit       ::= '0' | '1' | ... | '9'
letter      ::= 'A' | 'B' | ... | 'Z' | 'a' | 'b' | ... | 'z'
identifier  ::= letter { letter | digit | '_' }
type        ::= 'bool' | 'char' | 'int' | 'float' | 'double' | 'void' | 'wchar_t' | 'string' | 'Error'
value       ::= digit { digit } | '"' { any_character } '"'
```

#### 2. Variable Declaration

```
variable_declaration ::= type identifier '=' value ';'
constant_declaration ::= 'const' type identifier '=' value ';'
```

#### 3. Functions

```
function_declaration ::= ['export'] type identifier '(' [parameters] ')' '{' { statement } '}'
parameters           ::= parameter { ',' parameter }
parameter            ::= type identifier
statement            ::= (variable_declaration | expression | console_log) ';'
console_log          ::= 'console.log' '(' expression ')'
```

#### 4. Classes and Inheritance

```
class_declaration   ::= 'class' identifier [extends_clause] '{' { class_member } '}'
extends_clause      ::= 'extends' identifier
class_member        ::= constructor | method
constructor         ::= 'constructor' '(' [parameters] ')' '{' { statement } '}'
method              ::= type identifier '(' [parameters] ')' '{' { statement } '}'
```

#### 5. Interfaces

```
interface_declaration ::= 'interface' identifier '{' { interface_member } '}'
interface_member      ::= type identifier ';'
```

#### 6. Object Creation

```
object_creation ::= identifier identifier '=' identifier '(' [arguments] ')' ';'
arguments        ::= expression { ',' expression }
```

#### 7. Templates and Generics

```
generic_function ::= type identifier '<' type '>' '(' parameter ')' '{' { statement } '}'
```

#### 8. Async/Await for Threading

```
async_function ::= 'async' type identifier '(' [parameters] ')' '{' { async_statement } '}'
async_statement ::= 'await' expression ';' | statement
```

#### 9. Error Handling

```
try_catch_block ::= 'try' '{' { statement } '}' 'catch' '(' 'Error' identifier ')' '{' { statement } '}'
```

#### 10. Modules and Imports

```
import_statement ::= 'import' '{' import_list '}' 'from' string_literal ';'
import_list      ::= identifier { ',' identifier }
```

#### 11. Pointers and References

```
pointer_reference ::= 'copy' '(' expression ')'
```

#### 12. NULL Handling

```
null_reference ::= identifier '=' 'null' ';'
```

#### 13. Printing to Console

```
console_log ::= 'console.log' '(' expression ')' ';'
```

#### 14. Receiving Input

```
input_statement ::= type identifier '=' 'input' '(' ')' ';'
```

#### Miscellaneous

```
expression    ::= identifier | value | function_call | object_creation | input_statement | pointer_reference
function_call ::= identifier '(' [arguments] ')'
```
