# Lua 5.2 Reference:
[Official Docs](lua.org/documentation)  
[Lua on Wikipedia](https://en.wikipedia.org/wiki/Lua_(programming_language))  

## Introduction
<details>
<summary>Overview</summary>

* Supports general procedural programming.  
* Offers support for: 
  * oop
  * functional programming
  * data-driven programming
* Lua has **no** main().  
  * It only works embedded in a host application.  
  * The host can: 
    * Invoke functions to call lua code
    * Write and read lua variables
    * Register C functions to be called by lua.

* The Lua distribution includes a sample program called lua, which uses the Lua library to offer a complete interpreter, for interactive or batch use.
</details>

## Basic Concepts

### Values and Types
<details>
<summary>Values and Types</summary>

* Dynamically typed language:  
  * Variables have no type  
  * Values have type  
* Values are _first-class_.  Now, what does _first-class_ mean?
  * _first-class_ means that **all values** can be stored in variables, passed to other functions, and returned as results.
* Basic Types - there are 8:
  * nil:  
    Different than any other value - usually represents the absence of value.  
    Makes a condition false

  * boolean:  
    **false** and **true**  
    False makes a condition false (along with **nil**)  
    **any other value** means true (e.g. 0 is true, -1 is true, etc.)

  * number:  
    Represents real, (double-precision floating point) numbers  
    Usually follows the C implementation which is IEEE 754  

  * string:  
    Represents **immutable** sequences of bytes.  
    Lua is "8-bit clean".  Strings can contain any 8-bit value, including \0 (null).

  * function:  
    Lua can call (and manipulate) functions written in lua and functions written in C

  * userdata:  
    Userdata allows arbitrary C data to be stored in lua variables.  
    A userdata value is a pointer to a raw block of memory.  
    Two kinds of userdata:  
    * full userdata: the block of memory is managed by lua
    * light userdata: the block of memory is managed by the host.  
    Userdata has no pre-defined operations in lua, except assignment and identity testing.  
    By using metatables, you can define operations for full userdata values.  
    Userdata cannot be created or modified in Lua, only through the C api.

  * thread:  
    Represents independent threads of execution, and is used to implement **co-routines**.  
    Don't confuse lua threads with os threads.  
    Lua supports threads on all systems, even those systems who do not support threads natively.  

  * table:  
    Tables implement _**associative arrays**_:  
    * Can be indexed with numbers (like an array)  
    * Can be indexed with types (exclusing nil and NaN)  
    * Can be heterogeneous: contain values of **all** types (except nil)  
    * **Any key with value nil is not considered part of the table!**  
    * Any key that is not part of a table has an associated nil value.  
    * Are the sole data structure in all of lua.  They can represent:  
      * arrays  
      * sequences  
      * symbol tables  
      * sets  
      * records: to represent a record, lua uses the field name as an index.  (e.g. a.name is the same as a["name"])  
      * graphs  
      * trees  
  
  Tables, functions, threads, and full userdata are objects: variables **do not** contain these items, only references to them (e.g. pointers).  Assignment, parameter passing, and function returns always manipulate references to such values.  There is no implication of _any kind_ of copy.

</details>

### Environments and the Global Environment
<details>
<summary>The Global Environment (_ENV)</summary>

* _ENV:  
  Any reference to a global name _var_ is syntactically translated to _ENV.var  
  Every chunk is compiled in the scope of an external local variable called _ENV.  _ENV is never a global name in a chunk.  
* Any table used as the value of _ENV is called an **environment**  
* Lua keeps a _global environment_.  This value is kept at a special place in the *C registry*.  
  * _G is initialized with this same value  
* When lua compiles a chunk, it initializes the value of the chunk _ENV *upvalue* with the global environment. 
  * By default: global variables in lua refer to entries in the global environment.  
* All standard libraries are loaded in the global environment and several functions operate on that environment.  You can use **load** or **loadfile** to load a chunk with a different environment.  
* If you change the global environment in the registry (via C code or the debug library), all chunks loaded after the change will get the new environment.  
  * Previously loaded chunks are not affected, however, as each has its own reference to the environment in the chunks _ENV variable.  Moreover, the variable _G (which is stored in the original global ENV) is **never** updated by lua.  


</details>

### Error Handling
<details>
<summary>Error Handling</summary>

* All actions start from C code in the host program - calling a function from the lua library (lua_pcall).
* Errors during compilation or execution of a chunk returns control to host application, which takes appropriate steps.  
* Lua code can explicitly generate an error by using the *error* function.  
  * If you need to catch errors in lua, you can use pcall or xpcall to call a function in *protected mode*  
  * *error objects* (or message) are propagated with information about the error.  
  * xpcall or lua_pcall can receive a message handler to be called in case of errors.  This callback is called prior to stack unwinding, so it may gather a stack traceback.  
  
</details>

### Metatables and Metamethods

<details>
<summary>Metatables</summary>

* Every *value* in lua can have a metatable.  
  * A metatable is an ordinary lua table that defines the behavior of the original value under certain special operations.  
  * You can change several aspects of the behavior of operations over a value by setting specific fields in the values metatable.  
    * **Example:**
      * when a *non-numeric* value is the operand of an addition, lua checks for a function in the "__add" field of the value's metatable.  If it finds one, lua will call that function to do the addition.**
  * Keys in a metatable are derived from ***event names***
    * Corresponding values are called *metamethods*
    * In the example, the *event* is "add" and the metamethod is the function that performs the add.
  * Querying metatables:
    * *getmetatable* function
  * Replacing metatable of a table:
    * *setmetatable* function 
    * You can't change the metatable of other types from Lua (except by using the debug library)
  * Tables and full userdata have individual metatables
    * Values of all other types share one metatable per type  
      * One, single metatable for strings
      * One, single metatable for numbers, etc.
    * By default, a value has no metatable, but the string library sets a metatable for the string type
  * Metatables control how objects behave in arithmetic, order comparisons, concatenation, length checking and indexing.
    * Can also define a function that is called when userdata or table is garbage collected
      * Lua checks if the value has a metatable with corresponding event.
      * If so, value associated with that key (the metamethod) controls how lua will perform that operation.
  * Metatables control the operations listed.
    * Each operation has a corresponding name
    * Key for each operation is a string with name prefixed by 2 underscores
      * Example: key for the *add* operation is the string "__add"
  
  * **"add"**: the + operation
    ```lua
      -- the function getbinhandler defines how lua 
      -- chooses a handler for a binary operation.
      -- first, lua tries 1st operand.  If this type doesn't
      -- define a handler, the Lua tries the second
      function getbinhandler (op1, op2, event)
         return metatable(op1)[event] or metatable(op2)[event]
       end

       -- by using this function, the behavior of op1 + op2 is
       function add_event (op1, op2)
         local o1, o2 = tonumber(op1), tonumber(op2)
         if o1 and o2 then  -- both operands are numeric?
           return o1 + o2   -- '+' here is the primitive 'add'
         else  -- at least one of the operands is not numeric
           local h = getbinhandler(op1, op2, "__add")
           if h then
             -- call the handler with both operands
             return (h(op1, op2))
           else  -- no handler available: default behavior
             error(···)
           end
         end
       end
    ```
  * **"sub"**: the - operation
  * **"mul"**: the * operation
  * **"div"**: the / operation
  * **"mod"**: the % operation
  
    * o1 - floor(o1/o2)*o2 as primitive operation  
  * **"pow"**: the ^ operation
  
    * pow from C math library is primitive operation
  * *NOTE:* all the above operations behave the same as the "add" operation
  
  * **"unm"**: the unary - operation
    ```lua
       function unm_event (op)
       local o = tonumber(op)
       if o then  -- operand is numeric?
         return -o  -- '-' here is the primitive 'unm'
       else  -- the operand is not numeric.
         -- Try to get a handler from the operand
         local h = metatable(op).__unm
         if h then
           -- call the handler with the operand
           return (h(op))
         else  -- no handler available: default behavior
           error(···)
         end
       end
     end
    ```

  * **"concat"**: the .. operation
    ```lua
       function concat_event (op1, op2)
       if (type(op1) == "string" or type(op1) == "number") and
          (type(op2) == "string" or type(op2) == "number") then
         return op1 .. op2  -- primitive string concatenation
       else
         local h = getbinhandler(op1, op2, "__concat")
         if h then
           return (h(op1, op2))
         else
           error(···)
         end
       end
     end
    ```

  * **"len"**: the # operation
    ```lua
       function len_event (op)
       if type(op) == "string" then
         return strlen(op)      -- primitive string length
       else
         local h = metatable(op).__len
         if h then
           return (h(op))       -- call handler with the operand
         elseif type(op) == "table" then
           return #op              -- primitive table length
         else  -- no handler available: error
           error(···)
         end
       end
     end
    ```

  * **"eq"**: the == operation.  
    The function getequalhandler defines how Lua chooses a metamethod for equality.  A metamethod is selected only when both values being compared have the same type and the same metamethod for the selected operation - and the values are either tables or full userdata
    ```lua
       function getequalhandler (op1, op2)
       if type(op1) ~= type(op2) or
          (type(op1) ~= "table" and type(op1) ~= "userdata") then
         return nil     -- different values
       end
       local mm1 = metatable(op1).__eq
       local mm2 = metatable(op2).__eq
       if mm1 == mm2 then return mm1 else return nil end
     end
    ```

    The eq event is defined as follows (result is always a boolean):
    ```lua 
       function eq_event (op1, op2)
        if op1 == op2 then   -- primitive equal?
          return true   -- values are equal
        end
        -- try metamethod
        local h = getequalhandler(op1, op2)
        if h then
          return not not h(op1, op2)
        else
          return false
        end
       end
    ```

  * **"lt"**: the < operation
    ```lua
    function lt_event (op1, op2)
       if type(op1) == "number" and type(op2) == "number" then
         return op1 < op2   -- numeric comparison
       elseif type(op1) == "string" and type(op2) == "string" then
         return op1 < op2   -- lexicographic comparison
       else
         local h = getbinhandler(op1, op2, "__lt")
         if h then
           return not not h(op1, op2)
         else
           error(···)
         end
       end
     end
    ```

  * **"le"**: the <= operation  
    ```lua
    function le_event (op1, op2)
      if type(op1) == "number" and type(op2) == "number" then
        return op1 <= op2   -- numeric comparison
      elseif type(op1) == "string" and type(op2) == "string" then
        return op1 <= op2   -- lexicographic comparison
      else
        local h = getbinhandler(op1, op2, "__le")
        if h then
          return not not h(op1, op2)
        else
          h = getbinhandler(op1, op2, "__lt")
          if h then
            return not h(op2, op1)
          else
            error(···)
          end
        end
      end
    end
    ```

  * **"index"**: the indexing access table[key].  The metamethod is tried only when key is not present in table.  When table is not a table, no key is ever present, so the metamethod is always tried
    ```lua
    function gettable_event (table, key)
       local h
       if type(table) == "table" then
         local v = rawget(table, key)
         -- if key is present, return raw value
         if v ~= nil then return v end
         h = metatable(table).__index
         if h == nil then return nil end
       else
         h = metatable(table).__index
         if h == nil then
           error(···)
         end
       end
       if type(h) == "function" then
         return (h(table, key))     -- call the handler
       else return h[key]           -- or repeat operation on it
       end
    end
    ```

  * **"newindex"**: indexing assignment table[key] = value.  Metamethod is only tried with the key is not present in table.
    ```lua
    function settable_event (table, key, value)
       local h
       if type(table) == "table" then
         local v = rawget(table, key)
         -- if key is present, do raw assignment
         if v ~= nil then rawset(table, key, value); return end
         h = metatable(table).__newindex
         if h == nil then rawset(table, key, value); return end
       else
         h = metatable(table).__newindex
         if h == nil then
           error(···)
         end
       end
       if type(h) == "function" then
         h(table, key,value)           -- call the handler
       else h[key] = value             -- or repeat operation on it
       end
    end
    ```
  * **"call"**: when lua calls a value
    ```lua
    function function_event (func, ...)
      if type(func) == "function" then
        return func(...)   -- primitive call
      else
        local h = metatable(func).__call
        if h then
          return h(func, ...)
        else
          error(···)
        end
      end
    end

    ```
</details>

### Garbage Collection
<details>
<summary>Garbage Collection</summary>

* Lua manages memory automatically.
  * No need to allocate or free memory
  * Garbage collector collects all "dead objects"
  * All memory is subject to automatic management
    * strings, tables, userdata, functions, threads, internal structures, etc.
* Lua implements an incremental [*mark and sweep*](https://en.wikipedia.org/wiki/Tracing_garbage_collection)
  * Two numbers to control cycles:
    * *garbage-collector pause*
    * *garbage-collector step multiplier*
      * Both use percentages as units - value of 100 means internal value of 1
* Garbage-collector pause:  
  * controls how long the collector waits before starting a new cycle.
  * larger values make the collector less aggressive
  * values < 100 mean collector will not wait to start a new cycle
  * vale of 200 means collector waits for total memory in use to double before a new cycle starts
* Garbage-collector step multiplier:
  * controls the relative speed of the collector to memory allocation
  * larger values make the collector more aggressive, but also increase the size of each incremental step
  * values < 100 make the collector too slow, and can result in the collector never finishing a cycle.
  * default is 200, means that the collector runs at 2ce the speed of memory allocation
  * **Note:** if you set step multiplier to a very large number (> 10% of the max number of bytes the program may use), the collector acts like a "stop-the-world" collector.
* The numbers are set by calling lua_gc in C or collectgarbage in Lua
* Lua 5.2 can change the type of collector from incremental to generational.  
  * Generational collector assumes most objects die young
  * The generational collector performs a full collection from time to time.
</details>

<details>
<summary>Garbage collection metamethods</summary>

* You can set garbage-collector metamethods for tables (and using the C api) full userdata.
  * These are called "finalizers".
  * *finalizers* allow you to coordinate luas garbage collection with external resource management (e.g. closing files, network or db connections, freeing memory)
* For an object to be finalized when collected - you must **mark** it for finalization.  
  * Marking happens when you set its metatable and the metatable has a field indexed by the string "__gc"
* When a marked object becomes garbage, Lua puts it in a list.
  * After collection, Lua does the equivalent of the following for each object in the list:
  ```lua
  function gc_event (obj)
    local h = metatable(obj).__gc
    if type(h) == "function" then
      h(obj)
    end
  end
  ```

* At the end of each gc cycle, the finalizers for objects are called in reverse order that they were marked (LIFO).
  * Because the object being collected must still be used by the finalizer, Lua must ressurrect it.
* When you close a state (lua_close) lua calls the finalizers of all objects marked for finalization, following reverse order they were marked.
  
</details>

<details>
<summary>Weak tables</summary>

A weak table is a table whose elements are *weak references*.  A weak reference is ignored by the gc.
* If the only references to an object are weak references, then the garbage collector will collect that object.

* A weak table can have weak keys, weak values, or both.
  * A table with weak keys allows keys gc
  * A table with weak values allows values gc
  * A table with both weak keys and values allows gc of both.
* In any case, if either keys or values are collected, the whole pair is removed from the table.
  * The weakness of a table is controlled by the __mode field of it's metatable.  
  * If the __mode field is a string containing the letter 'k' the keys are weak. 
  * If the __mode field is a string containing the letter 'v' the values are weak.

* A table with weak keys and strong values is called an *ephemeron table*
  * Value is considered reachable only if the key is reachable

* Only objects that have an explicit construction are removed from weak tables.
  * Values, such as numbers and light C functions, are not subject to gc, and therefore not removed from tables (unless the associated value is collected)

* Resurrected objects are removed from weak values before running finalizers, but are removed from weak keys only in the next collection.

</details>



### Coroutines
<details>
<summary>Co-routines</summary>

* Co-routines are also called [*collaborative multi-threading*](https://en.wikipedia.org/wiki/Cooperative_multitasking)
* Coroutines represent independent threads of execution  
  * Unlike multi-threaded systems, a coroutine only suspends execution by explicitly calling a ***yield*** function.

* Coroutine Creation:
  * use coroutine.create
    * Sole argument is a function that is the main function of the coroutine.  
    * Create function only creates a new coroutine and returns a handle to it (an object of type ***thread***) - it does **not** start the coroutine.

* Coroutine Execution:
  * use coroutine.resume
    * argument is the handle generated by coroutine.create
    * coroutine starts execution at first line of it's main function
    * extra args passed to coroutine.resume are passed on to the coroutine main function.  
    * the coroutine runs until it terminates or ***yields***

  * Termination: 
    * normal termination: main function returns (explicitly or implicitly after last instruction)
      * coroutine.resume returns **true**
    * abnormal: unprotected error
      * coroutine.resume returns **false** plus error message

* Coroutine Yield:
  * coroutine.yield
    * When a routine yields, the corresponding coroutine.resume returns immediately - **even if the yield happened inside nested function calls**
    * coroutine.resume returns true, plus any values passed to coroutine.yield
  * The next time you resume the same coroutine, it continues execution from the point where it yielded, with the call to coroutine.yield returning any extra arguments passed to coroutine.resume.

* Coroutine Wrap:
  * coroutine.wrap
    * creates a coroutine, but returns a function that - when called - resumes the coroutine
    * Any args passed to this function go as extra arguments to coroutine.resume
    * returns all the values returned by coroutine.resume, except the first one (the boolean error code)
    * coroutine.wrap does not catch errors - any error is propogated to the caller.

**Example of co-routine**
```lua
function foo (a)
  print("foo", a)
  return coroutine.yield(2*a)
end

co = coroutine.create(function (a,b)
      print("co-body", a, b)
      local r = foo(a+1)
      print("co-body", r)
      local r, s = coroutine.yield(a+b, a-b)
      print("co-body", r, s)
      return b, "end"
end)

print("main", coroutine.resume(co, 1, 10))
print("main", coroutine.resume(co, "r"))
print("main", coroutine.resume(co, "x", "y"))
print("main", coroutine.resume(co, "x", "y"))

--[[
    When you run it, it produces the following output:

     co-body 1       10
     foo     2
     main    true    4
     co-body r
     main    true    11      -9
     co-body x       y
     main    true    10      end
     main    false   cannot resume dead coroutine
--]]
```

**Note:** you can also create and manipulate coroutines through the C api: lua_newthread, lua_resume, lua_yield.

</details>

## The Language
<details>
<summary>Lexis, syntax, and semantics</summary>

* Which tokens are valid, how they can be combined, and what the combinations mean.
* Constructs in [extended BNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form)
  * {a} means 0 or more a
  * [a] means optional a

</details>

### Lexical Conventions
<details>
<summary>Lexical Conventions</summary>

* Lua is a free-form language.
  * Ignores spaces (including new-lines) and comments between lexical elements (tokens)
  * Names in lua can be any string of letters, digits, and underscores, not beginning with a digit
</details>

<details>
<summary>keywords</summary>

* **keywords**: case-sensitive.  and, And, AND are all different, and only one is reserved (and).  
  ```
  and  
  false  
  local   
  then  
  break  
  for  
  nil  
  true  
  do  
  function  
  not  
  until  
  else  
  goto  
  or  
  while  
  elseif  
  if  
  repeat  
  end  
  in  
  return  
  ```
</details>

<details>
<summary>Other tokens</summary>

* **other tokens**  
    ```
    +  
    -  
    *  
    /  
    %  
    ^  
    #  
    ==  
    ~=  
    <=  
    >=  
    <  
    >
    =  
    (  
    )  
    {  
    }  
    [  
    ]  
    ::  
    ;  
    :  
    ,  
    .  
    ..  
    ...  
    ```
</details>

<details>
<summary>Literal strings</summary>

* **literal strings**  
  * Can be delimited by matching single or double quotes
  * Can contain c-like escapes
  ```
    \a (bell), 
    \b (backspace),
    \f (formfeed),
    \n (newline),
    \r (carriage return),
    \t (horizontal tab),
    \v (vertical tab),
    \\ (backslash),
    \" (double-quote),
    \' (single-quote)
    \z (skip the following span of whitespace characters, including line breaks)
  ```
  * Bytes in literal strings can be specified by numerical values:
    * escape sequence **\xXX** where XX is 2 hex digits, or
    * escape sequence **\ddd** where ddd is up to 3 decimal digits
    * **\0** embedded 0

  * Long-bracket literal string form
    * Levels 
      * (level 0): [[]]
      * (level 1): [=[]=]
      * (level 2): [==[]==]
      * etc.
  * Example - these are all the same output / string:
  ```lua
  a = 'alo\n123"'

  b = "alo\n123\""

  c = '\97lo\10\04923"'

  d = [[alo
  123"]]

  e = [==[
  alo
  123"
  ]==]
</details>

<details>
<summary>Numerical Constants</summary>

* Numerical Constants
  * Can be written with optional fractional part and optional decimal exponent, marked by letter 'e' or 'E'.
  * Lua also accepts hex constants, which start with 0x or 0X
    * Hex constants also accept an optional fractional part plus an optional binary exponent, marked by letter 'p' or 'P'
  * Examples:
  ```lua
  3 
  3.0
  3.1416
  314.16e-2
  0.31416E1
  0xff
  0x0.1E
  0xA23p-4
  0x1.921FB54442D18P+1
  ```
</details>

<details>
<summary>Comments</summary>

* Comments
  * start with double hyphen (--), 
  * if the text immediately after is a [, then it's a "long comment".  otherwise, it's a short comment.
  
</details>

### Variables
<details>
<summary>Variables</summary>

**eBNF:**
```bnf 
  var ::= Name
```

* There are 3 kinds of variables in Lua:
  * global
  * local
  * table fields

    Variable names are assumed to be global unless explicitly marked local.  Local variables are lexically scoped - they can be freely accessed by functions defined inside their scope.  

    Before the first assignment to a variable, its value is **nil**.

* Square brackets are used to index a table:  
    **eBNF**
    ```bnf
    var ::= prefixexp '[' exp ']'
    ```

    The meaning of accesses to table fields can be changed via metatables.  An access to an indexed variable t[i] is equivalent to a call gettable_event(t, i).  This is not defined or available in Lua

* The syntax var.Name is just syntax sugar for var["Name"]:

    **eBNF**
    ```bnf
    var ::= prefixexp '.' Name
    ```

* An access to a global variable X is equivalent to _ENV.x
* Due to the way chunks are compiled, _ENV is **never** a global name.

</details>


### Statements
<details>
<summary>Statements</summary>

Lua supports almost all conventional set of statements - similar to Pascal or C.  This set includes:
* assignments
* control structures
* function calls
* variable declarations

</details>

#### Blocks
<details>
<summary>Blocks</summary>

A block is a list of statements, which are sequentially executed:  
**eBNF**
```bnf
block ::= {stat}
```

Lua has empty statements that allow to separate statements with semicolons, start a block with a semicolon, or write 2 semicolons in sequence:  
**eBNF**
```bnf
stat ::= ';'
```

Function calls and assignments can start with an open paren.  This leads to an ambiguity.  
Consider:
```lua
a = b + c
(print or io.write)('done')
```

The grammar could see it in two ways:
```lua
a = b + c(print or io.write)('done')  -- OR
a = b + c; (print or io.write)('done')
```

The parser sees it the first way.  It interprets the open paren as the start of arguments to a function call.  Therefore, 
**always precede statements that start with a paren with a semi-colon**

A block can be explicitly delimited to produce a single statement:  
**eBNF**
```bnf
stat ::= do block end
```

Explicit blocks are useful to control the scope of variable declarations.  Explicit blocks are also sometimes used to add a **return** statement in the middle of another block.  
</details>

#### Chunks
<details>
<summary>Chunks</summary>

A unit of compilation in lua is a chunk.  Syntactically, a chunk is just a block.  
**eBNF**
```bnf
chunk ::= block
```

Lua handles a chunk as the body of an anonymous function with a variable number of arguments.  
So, chunks can define local variables, receive arguments, and return values.  
And, this anonymous function is compiled as in the scope of an external local variable called _ENV.  The resulting function always has _ENV as its only **upvalue**, even if it does not use that variable.

A chunk can be stored in a file, or in a string inside the host program.  To execute a chunk Lua first precompiles the chunk into instructions for a VM, and then it executes the compiled code with an interpreter for the VM.

Chunks can also be precompiled into binary form.  You would use **luac** to do this.  Programs in source and compiled forms are interchangeable, lua automatically detects the type and acts accordingly.
</details>

#### Assignment
<details>
<summary>Assignment</summary>

Lua allows multiple assignment.  Ergo, the syntax for assignment defines a list of variables on the left side, and a list of expressions on the right side.  The elements in both are separated by commas.  
**eBNF**
```bnf
stat ::= varlist '=' explist
varlist ::= var {',' var}
explist ::= exp {',' exp}
```

Before the assignment, the list of values (right-side) is adjusted to the length of the list of variables (left-side).  
If there are more values than needed, the excess values are thrown away.  
If there are fewer values than needed the list is extended with as many **nils** as necessary.  
If the list of expressions ends with a function call, the all values returned by that call enter the list of values, *before* the adjustment (except when the call is enclosed in parens).  
The assignment statement first evaluates all its expressions and only hten are the assignments performed.  
Thus:
```lua 
i = 3
i, a[i] = i+1, 20
```
sets a[3] to 20, without affecting a[4] because i in a[i] is evaluated (to 3) before it's assigned 4.  Similarly, the line  
```lua
x, y = y, x         --exchanges the values of x and y, and 
x, y, z = y, z, x   --cyclically permutes the values of x, y, and z.
```

The meaning of assignments to global variables and table fields can be changed via metatables.  An assignment to an indexed variable t[i] = val is equivalent to *settable_event(t, i, val)*.  Note: this function is not defined or callable in lua.

An assignment to a global variable x = val is equivalent to the assignment _ENV.x = val
</details>

#### Control Structures
<details>
<summary>Control Structures</summary>

The control structures: **if**, **while**, and **repeat** have the usual meaning.  
**eBNF**
```
stat ::= while exp do block end
stat ::= repeat block until exp
stat ::= if exp then block {elseif exp then block} [else block] end
```

Lua has a **for** statement, of 2 types:  
 * generic for
 * numeric for

The condition expression of a control structure can return any value.  Both **false** and **nil** are considered false.  All values different from **nil** and **false** are considered true (specifically, 0 and empty string are **true**).  

In the **repeat-until** loop, the inner block doesn't end at the until keyword, but only after the condition.  So the condition can refer to local variables declared inside the loop block.  

The **goto** statement transfers program control to a label.  For syntax reasons, labels in lua are statements as well:  
**eBNF**
```bnf
stat ::= goto Name
stat ::= label
label ::= '::' Name '::'
```

a label is visible in the entire block where it is defined, except inside nested blocks where a label with the same name is defined and inside nested functions.  A goto may jump to any visible label as long as it does not enter into the scope of a local variable.  

Labels and empty statements are called **void statements** as they perform no actions.  

The **break** statement terminates the execution of a **while**, **repeat**, **for** loop, skipping the next statement after the loop:  
**eBNF**
```bnf
stat ::= break
```

A break ends the innermost enclosing loop.

The **return** statement is used to return values from a function or a chunk (which is a function in disguise).  Functions can return more than one value, so the syntax for the return statement is:  
**eBNF**
```bnf
stat ::= return [explist] [';']
```

The return statement can only be written as the last statement of a block.  

If, however, you really need to return in the middle of a block, then an explicit inner do return end block can be used.

</details>

#### The For Statement
<details>
<summary>For Statement</summary>

* Generic For
* Numeric For

The **numeric for** repeats a block of code while a control variable runs through a progression.  It has the following syntax:  
**eBNF**
```bnf
stat ::= for Name '=' exp ',' exp [',' exp] do block end
```

The block is repeated for name starting at the value of the first exp, until it passes the second exp by steps of the 3rd exp.  
```lua
for v = e1, e2, e3 do block end

-- is equivalent to:
do 
  local var, limit, step = tonumber(e1), tonumber(e2), tonumber(e3)
  if not (var and limit and step) then error() end
  while (step > 0 and var <=limit) or (step <=0 and var >= limit) do
    local v = var
    block
    var = var + step
  end
end
```

* all 3 control expressions are only evaluated once, before the loop starts.  They **must** all result in numbers.
* var, limit and step are invisible variables
* if the third expression is absent, then a step of 1 is used.
* you can use **break** to exit a for loop
* The loop variable V is local to the loop.  You *cannot* use its value after the for ends or is broken.  If you need this value, assign it to a different variable.

The **generic for** works over functions, called iterators.  On each iteration, the iterator function is called to produce a new value, stopping when this new value is **nil**.  
**eBNF**
```bnf
stat ::= for namelist in explist do block end
namelist ::= Name {',' Name}
```

This:  
```lua
for var_1, ..., var_n in explist do block end

--is equivalent to
do 
  local f, s, var = explist
  while true do
    local var_1, ..., var_n = f(s, var)
    if var_1 == nil then break end
    var = var_1
    block
  end
end
```

* explist is evaluated only once.  It results are an iterator function, a state and an initial value for the first iterator variable.
* f, s, and var are invisible variables.  
* you can use break to exit a for
* The loop variables var_i are local to the loop.  Can't use them after the for ends.

</details>

#### Function calls as statements
<details>
<summary>Function calls as statements</summary>

To allow possible side effects, function calls can be executed as statements:  
**eBNF**
```bnf
stat ::= functioncall
```

In this case, all returned values are thrown away.

</details>

#### Local Declarations
<details>
<summary>Local declarations</summary>

Local variables can be declared *anywhere* inside a block.  The declaration can include an initial assignment:  
**eBNF**
```bnf
stat ::= local namelist ['=' explist]
```

if present, an initial assignment has the same semantics of a multiple assignment.  Otherwise, all variables are initialized to **nil**.  

A chunk is also a block, and so local variables can be declared in a chunk outside an explicit block.

</details>

## Expressions

### Basic Expressions
<details>
<summary>Basic Expressions</summary>

```bnf
exp ::= prefixexp
exp ::= nil | false | true
exp ::= Number
exp ::= String
exp ::= functiondef
exp ::= tableconstructor
exp ::= '...'
exp ::= exp binop exp
exp ::= unop exp
prefixexp ::= var | functioncall | '(' exp ')'
```

Binary operators: arithmetic operators, relational operators, logical operators and concatenation operator.

Both function calls and vararg (...) expressions can result in multiple values.  If a function call is used as a statement, the its return list is adjusted to zero elements, discarding all returned values.  If an expression is used as the last (or only) element of a list of expressions, then no adjustment is made (unless the expression is enclosed in parens.  In all other context, lua adjusts the result list to one element - either discarding all others, or a nil if there aren't any.

**Example:**
```lua
f()            -- adjusted to 0 results
g(f(), x)      -- f() is adjusted to 1 result
g(x, f())      -- g gets x plus all results from f()
a,b,c = f(), x --f() is adjusted to 1 result (c gets nil)
a,b = ...      --a gets first vararg, b gets second (a and b can both get nil)
a,b,c = x, f() --f() is adjusted to 2 results
a,b,c = f()    --f() is adjusted to 3 results
return f()     --returns all results from f()
return x,y,f() --returns x,y and all results from f()
{f()}          --creates a list with results from f()
{...}          --creates a list with all vararg parameters
{f(), nil}     --f() is adjusted to 1 result
```

Any expression in parens always results in only one value.  So: (f(x,y,z)) is always a single value, even if f returns several values.  The value is 1st result of f() or nil if f doesn't return anything.

</details>

### Arithmetic operators
<details>
<summary>Arithmetic Ops</summary>

Lua supports usual arithmetic operators:  
+, -, *, /, %, ^, and unary - (negation)  

If the operands are numbers or strings that can be converted to numbers, then all operators have the usual meaning.  Exponentiation works for any exponent.  E.g. x^(-0.5) is inverse of square root of x.  

Modulo is defined as:  
a % b == a - math.floor(a/b)*b  --remainder of division that rounds quotient to -inf.

</details>

### Coercion
<details>
<summary>Coercion</summary>

Lua automatically converts between string and number values at run time.  Any arithmetic operation applied to a string tries to convert this string to a number.  Whenever a number is used where a string is expected, the number is converted to a string in a reasonable format.

</details>

### Relational Operators
<details>
<summary>Relational Operators</summary>

The relational operators are:  
==   ~=   <   >   <=   >=  
They always return either false or true.  

Equality first compares the type of operands.  
* if types are different, result is **false**
* Otherwise, values of operands are compared  
* Numbers and strings are compared in the usual way.  
* Tables, userdata and threads are compared by **reference** they are only considered equal if they are *the same object* (memory address is the same)  
* Closures with the same reference are always equal.  
* Closures with any detectable difference are always different.  

You can change how lua compares tables and userdata by using the __eq metamethod  

Conversion rules **do not apply** to equality comparisons.  
* "0" == 0 --is **false**.
* t[0] and t["0"] denote different entries in the same table
* ~= is exactly the negation of ==
</details>

### Logical Operators
<details>
<summary>Logical Operators</summary>

They are: **and** **or** and **not**  
* All logical operators consider false and nil as false, and everything else as true  

* negation **not** always returns either false or true  
* **and** returns its first argument if this value is **false or nil**, otherwise, it returns the second argument  
* **or** returns its first argument if this value is different from **nil and false**.  Otherwise, it returns the 2nd argument.
* both and and or use short-cut evaluation.  the second operand is only evaluated if necessary.
</details>

### Concatenation
<details>
<summary>Concat</summary>

String concatenation is denoted by 2 dots **..**  If both operands are strings or numbers, then they are converted to strings.  Otherwise, the metamethod __concat is called.  
</details>

### The Length Operator
<details>
<summary>Length Operator</summary>

* The length operator denoted by unary prefix **#**.  Length of a string is its number of bytes.  
* You can use the __len metamethod to modify the length operator behavior for *any value but strings*.  
* Unless __len is given, the length of a table t is only defined if a table is a sequence.  The set of positive numeric keys is equal to {1..n} for some non-negative n.  
  * A table like {10, 20, nil, 40} is *not a sequence*
  * Non-numeric keys do not interfere with whether a table is a sequence.  E.g.: {10, 20, ['foo']='bar', 40} is len 3.  

</details>

### Precedence
<details>
<summary>Precedence</summary>

Operator precedence follows the table below from lower to higher priority:
```lua
or  
and  
<  >  <=  >=  ~=  ==  
..  
+ -
* / %
not # -
^  
```  

* Concatenation and exponentiation are right associative.  All others are left associative.  
* You can use parens to change the order of operations.  
</details>

### Table Constructors
<details>
<summary>Table Constructors</summary>

* Table constructors are expressions that create tables.  Every time a constructor is evaluated, a new table is created.  
  * You can either create an empty table, or an initialized one with values (or keys/values)  
**eBNF**
```bnf
tableconstructor ::= '{' [fieldlist] '}'
fieldlist        ::= field {fieldsep field} [fieldsep]
field            ::= '[' exp ']' '=' exp | Name '=' exp | exp
fieldsep         ::= ',' | ';'
```

* each field of the form [exp1] = exp2 adds to the new table an entry with the key exp1 and value exp2.
* a field in the form name = exp is equivalent to ["name"] = exp.
* Fields of the form exp are equivalent to [i] = exp where i are consecutive numerical integers, starting with 1.  Fields in other formats do not affect this counting.  
**Example**
```lua
a = { [f(1)]=g; "x", "y"; x=1, f(x), [30] = 23; 45 }

--- is equivalent to:
do 
  local t = {}
  t[f(1)] = g
  t[1] = "x"
  t[2] = "y"
  t.x = 1
  t[3] = f(x)
  t[30] = 23
  t[4] = 45
  a = t
end 
```

* If the last field in the list has the form *exp* and the expression is a function call or vararg expression, then all values returned by this expression enter the list consecutively.  
* The field list can have an optional trailing field separator.
</details>

### Function Calls
<details>
<summary>Function calls</summary>

Function calls have the following syntax:  
**eBNF**
```bnf
functioncall ::= prefixexp args
```

* First, prefixexp and args are evaluated.
  * If value of prefixexp has type *function*, then this function is called with given args
  * Otherwise, prefixexp __call metamethod is called - having as first parameter the value of prefixexp, followed by original call arguments.  

The form: 
**eBNF**
```bnf
functioncall ::= prefixexp ':' Name args
```

Can be used to call methods.  A call v:name(args) is the **exact same** as v.name(v, args), except that *v* is only evaluated once.  

Arguments have the following syntax: 
**eBNF**
```bnf
args ::= '(' [explist] ')'
args ::= tableconstructor
args ::= String
```

All argument expressions are evaluated before the call.  
* A call of form f{fields} is sugar for f({fields}).
* a call of form f'string' or f"string" or f[[string]] is the same as f('string').

A call of the form *return functioncall* is called a **tail call**.  

Lua implements proper tail calls (or *tail recursion*).  In a tail call: 
* the called function reuses the stack entry of the calling function.
* There is no limit on the number of nested tail calls that a program can execute.
* Tail calls erase any debug information about the calling function.
* Tail calls **only happen** when return has *one single function call* as argument.  None of the following are tail calls:  

**Example**
```lua
return (f(x))   -- results adjusted to 1
return 2 * f(x) -- 
return x, f(x)  -- additional results
f(x); return    -- results discarded
return x or f(x) -- results adjusted to 1
```

</details>

### Function Definitions
<details>
<summary>Function Defs</summary>

Syntax:  
**eBNF**
```bnf
functiondef ::= function funcbody
funcbody ::= '(' [parlist] ')' block end
```

Syntactic sugar forms:
**eBNF**
```bnf
stat ::= function funcname funcbody 
stat ::= local function Name funcbody
funcname ::= Name {'.' Name} [':' Name] 
```

* Function statements and translations
```lua
function f() body end         -- translation f = function() body end
function t.a.b.c.f() body end -- translation t.a.b.c.f = function() body end
local function f() body end   -- translation local f; f = function() body end
```

* Function definitions are executable expressions, whose *value* has type function.  
  * When lua precompiles a chunk, all the function bodies are compiled too.
  * When Lua executes the function definition,the function is *instantiated* (or closed).
  * This function closure is the final value of the expression.

* Parameters act as local variables initialized with the argument values:
**eBNF**
```bnf
parlist ::= namelist [',' '...'] | '...'
```

* When a function is called, the list of arguments is adjusted to the length of the list of parameters, unless the function is a *vararg function*, which is indicated by three dots (...) at the end of the parameter list.  
  * Vararg functions do not adjust their argument list.  It collects all the arguments and supplies them to the function through a *vararg expression*, which is also 3 dots (...)
  * The value of the expression is a list of all actual extra arguments, similar to a function with multiple results.
  * If a vararg expression is used inside another expression or in the middle of a list of expressions, the its return list is **adjusted to one element**.
  * If the expression is used as the last element of a list of expressions, then no adjustment is made (unless the last expression is enclosed in parens)

**Example**
```lua
function f(a, b) end
function g(a, b, ...) end
function r() return 1,2,3 end

--- Call Mapping
--- CALL          PARAMETERS
f(3)              a=3, b=nil
f(3, 4)           a=3, b=4
f(3, 4, 5)        a=3, b=4
f(r(), 10)        a=1, b=10
f(r())            a=1, b=2

g(3)              a=3, b=nil, ... -> (nothing)
g(3, 4)           a=3, b=4, ... -> (nothing)
g(3, 4, 5, 8)     a=3, b=4, ... -> 5 8
g(5, r())         a=5, b=1, ... -> 2 3
```

* Results are returned using **return** statement.  If control reaches end of function without encountering a return, then function returns with no results.
* There is a system dependent limit on number of return values.  The limit is guaranteed to be larger than 1000.
* The colon synta is used for defining *methods*.  Functions that have an implicit extra parameter "self".  Thus:

```lua
function t.a.b.c:f (params) body end

-- is the same as
t.a.b.c.f = function(self, params) body end
```

</details>

### Visibility Rules
<details>
<summary>Visibility Rules</summary>

* Lua is lexically scoped.
  * The scope of the innermost variable begins at the first statement after its declaration and lasts until the last non-void statement of the innermost block that includes the declaration.
**Example**
```lua
x = 10                --global x
do                    -- new scope
  local x = x         -- new x, equivalent to global x (10) NOTE: new x **not yet in scope**, so value refers to outer (global, in this case) x
  print(x)            -- --> 10
  x = x+1             -- x == 11
  do                  -- new scope
    local x = x+1     -- references previous scope x (11)
    print(x)          -- --> 12
  end
  print(x)            -- outer inner scope x (11)
end
print(x)              -- back to global x
```

* Because of lexical scoping rules, local variables can be freely accessed by functions defined inside their scope.
* *A local variable used by an inner function* is called an **upvalue**  (or an external local variable), inside the inner function.
* Notice that each execution of a **local** statement defines new local variables. 
**Example**
```lua
a = {}
local x = 20
for i=1,10 do 
  local y = 0
  a[i] = function() y=y+1; return x+y end
end
```

* The loop creates 10 closures (10 instances of the anonymous function).  Each closure uses a different y variable, while all of the closures use the same x
</details>

## The Application Programming Interface

This will describe the C API for Lua.  
* C API is the set of C functions available to the host program to communicate with Lua.
* **All API functions and related types and constants are declared in header file: lua.h**
* Any facility in the API may be provided as a macro instead.
* Lua API functions do not check their arguments for validity or consistency.  You can use LUA_USE_APICHECK as compilation flag to change this.

### The Stack
<details>
<summary>The Stack</summary>


</details>
XXXXX This is where i finished for the night.

## Lua Standard Libraries
### Basic Library:

<details>
<summary>Basic Library</summary>

[_ENV]()  
[_G]()  
[_VERSION]()  

[assert(v [, message])](./lua/52/examples/assert.md)  
[error(message [, level])]()  
[print(...)]()  
[type(v)]()  
[tostring(v)]()  
[tonumber(e [, base])]()  

[ipairs(t)]()  
[pairs(t)]()  
[next (table [, index])]()  
[select(index, ...)]()  

[getmetatable(object)]()  
[setmetatable(table, metatable)]()  

[dofile([filename])]()  
[load (ld [, source [, mode [, env]]])]()  
[loadfile ([filename [, mode [, env]]])]()  

[rawequal(v1, v2)]()  
[rawget(table, index)]()  
[rawset(table, index, value)]()  
[rawlen(v)]()  

[pcall(f [, arg1, ...])]()  
[xpcall(f, msgh [, arg1, ...])]()  

[collectgarbage([opt [, arg]])](./lua/52/examples/collectgarbage.md)

</details>
