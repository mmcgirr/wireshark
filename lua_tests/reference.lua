#!/usr/local/bin/lua

--[[
 Block comment
--]]

--[[
    lvalue: expressions that refer to a memory location.  
    can be either left or right hand side of assignment
    rvalue: refers to a data value that is stored at some address in memory.
    an rvalue cannot have a value assigned to it (for example, 10 is an rvalue)
]]

--[[
    Lua types:
    Variables don't have types.  But the values have types.
    nil: value has data or doesn't have data
    boolean: true and false as values
    number: real (double-precision floats)
    string: array of characters
    function: method written in c or lua
    userdata: arbitrary C data
    thread: independent threads of execution - used for coroutines
    table: only data structure: used for arrays, symbol tables, sets, records,
        graphs, trees.  Implements associative arrays.  Can hold any value but nil  
]]

--[[
    Operators:
    + adds two operands
    - subtract second op from first
    * multiply both operands
    / divide numerator by denominator
    % modulus - remainder after int division
    ^ exponent
    - unary minus - negation
]]

--[[
    Relational Operators:
    == equality of the operands - if equal, result is true
    ~= inequality of operands - if not equal, result is true
    >  left operand greater than right - if yes, result true
    <  left operand lesser than right - if yes, result true
    >= left operand greater or equal to right - if yes, result true
    <= left operand lesser than right - if yes, result true
]]

--[[
    Logical Operators:
    and  if both operands are non-zero, then result is true
    or   if either operand is non-zero, then result is true
    not  if result is true, this will make it false
]]
--[[
    Misc Operators:
    ..   concatenates two strings
    #    unary operator that returns length of string or table
]]

--[[
    while loop:  repeats a statement (or group) while condition is true -
        condition test happens before execution of body
    for loop:    sequence of statements multiple times until some terminal 
        condition
    repeat...until: repeats operation until condition is met
    nested loops:  can nest any of the loop types

    break     terminates the loop and xfers execution to the statement 
        immediately following the loop or switch

    infinite loop
    while(true)
    do 
        print("Forever")
    end
]]

--[[
    Decisions / Branching:
    if   :   if condition, then statements
             any boolean true and non-nil is true
             boolean false or nil are considered false
             zero is considered true!
    if...else : additional condition check if initial if fails
    
]]

--[[
    Functions:
    (scope) function function_name(arg1, arg2 ... argN)
        function body
        return result(s)
    end

    Functions can be variadic using the ... operator.
    for example:

    function average(...)
        result = 0
        local arg = {...}
        for i,v in ipairs(arg) do
            result = result + v
        end
        return result/#arg
    end

    print("The average is", average(10,5,3,4,5,6))
]]

--[[
    Strings:
    Initialization:
        'chars'
        "chars"
        [[chars]]
--]]
--[[
    Escape chars:
    \a - Bell
    \b - Backspace
    \f - formfeed
    \n - newline
    \r - carriage return
    \t - tab
    \v - vert. tab
    \\ - backslash
    \" - doublequotes
    \' - singlequotes
    \[ - left bracket
    \] - right bracket
]]
--[[
    String Manipulations:
    string.upper(arg)
    string.lower(arg)
    string.gsub(mainstring, findstring, replacestring)
    string.find(mainstring, findstring, [startindex], [endindex])
    string.reverse(arg)
    string.format(...)
    string.char(arg)
    string.byte(arg)
    string.len(arg)
    string.rep(string, n)  --repeat a string n times
    .. concatenates strings
]]

--[[
    Arrays:
    One dimensional: 
        array = {"Lua", "Tutorial"}
        for i=0, 2 do
            print(array[i])
        end
    
]]

--[[
    Iterators:
    Stateless:
    Stateless Iterators

    By the name itself we can understand that this type of iterator function
    does not retain any state.
    Let us now see an example of creating our own iterator using a simple 
    function that prints the squares of n numbers.

    Stateful: 
    Stateful Iterators

    The previous example of iteration using function does not retain the state. 
    Each time the function is called, it returns the next element of the collection 
    based on a second variable sent to the function. 
    To hold the state of the current element, closures are used. 
    Closures retain variables values across functions calls. 
    To create a new closure, we create two functions including the closure itself 
    and a factory, the function that creates the closure.

    Let us now see an example of creating our own iterator in which we will be using closures.
]]

--[[
    Tables:
    Only data structure available in lua.
    Associative arrays, which can be indexed with both numbers and strings.  Cannot use nil

    Lua uses tables in all representations including packages.  When we access the string.format method
    we are accessing the format function available in the string package.
    
    Tables are called objects, and are neither values nor variables.  Lua uses a constructor 
    expression {} to create an empty table.  
    There is no fixed relationship between the variable that holds the reference to the table and 
    the table itself.

    If you assign a table to "nil" e.g. mytable = nil, then lua's GC will handle disposing of the 
    memory

    If we have a table a, and assign it to b, then both a and b refer to the same memory.  NO separate
        memory is allocated for b
    When a is set to nil, the table is still accessible by b.
    When there are no references left to the table memory, then GC will clean up the process and make
        the unreferenced memory available again.    
]]

--[[]]
---
--    Table Manipulations:
--    table.concat(table [, sep [, i [,j]]]) : concatenates strings in tables based on params
--    table.insert(table, [pos,] value) : insert value into table at pos
--    table.maxn(table) : returns largest numeric index
--    table.remove(table [, pos]) : removes value from table at pos
--    table.sort(table [, comp]) : sorts the table based on optional comparator argument.
---
--]]

--[[
    Modules:
    Modules are like libraries
    They are loaded using "require", and have a single global name containing a table.

    The module can contain variables and functions.  All functions and variables are wrapped in the table.
    The table, then, acts as a namespace.
    
    Modules have provisions to return the table on "require"

    A user can call module functions in multiple ways.  For example:
    module printFormatter - contains function simpleFormat(arg)
    
    -- Type 1
    require('printFormatter')
    printFormatter.simpleFormat("test")

    -- Type 2
    local formatter = require('printFormatter')
    formatter.simpleFormat("test")

    -- Type 3
    require('printFormatter')
    local formatterFunction = printFormatter.simpleFormat
    formatterFunction("test")

    Require:
    require just assumes the module is a chunk of code that defines some values, which are
    actually functions, or tables containing functions.

    Example:
    -- mymath.lua
    local mymath = {}  -- define the module
    
    function mymath.add(a,b)
        print(a+b)
    end

    function mymath.sub(a,b)
        print(a-b)
    end

    function mymath.mul(a,b)
        print(a*b)
    end

    function mymath.div(a,b)
        print(a/b)
    end
    return mymath  -- make it usable

    -- in someotherlua.lua
    mymathmod = require("mymath")
    mymathmod.add(10,20)
    ...etc.

    NOTE: the two files above would need to be in the same directory.  OR, place the module in 
    the "package.path" of lua.
    NOTE2: module name and file name should be the same (e.g. mymath.lua)
    NOTE3: return modules so "local mymath={} .... return mymath"
]]

--[[
    Metatables:
    A metatable is a table that helps in modifying the behavior of a table the metatable is 
    attached to - with the help of a key set and related meta methods.
    Meta methods enable features like:
    * changing / adding functionality to operators on tables.
    * looking up metatables when the key is not available in the table using __index in metatable

    setmetatable(table, metatable) - set the metatable for a table
    getmetatable(table) - get the metatable of a table

    __index - lookup value in metatable when not available in table
    __newindex - if keys aren't available in table, the behavior of new keys is defined by meta methods
    
]]

local function basic()
    print("------------basic------------")

    print("test")
    io.write("Hello world, from ", _VERSION, "!\n")
end

local function types()
    print("------------types------------")

    print(type("What is my type"))
    local t=10
    local x
    print(type(5.8*t))
    print(type(true))
    print(type(print))
    print(type(nil))
    print(type(type(ABC)))
    print("x type = " .. type(x))
    --print("x =" .. x) -- error, x is nil
end

local function strings()
    print("------------strings------------")

    local string1 = "Lua"
    local string2 = 'Tutorial'
    local string3 = [[Lua Tutorial \n]]
    print("String 1: ", string1)
    print("String 2: ", string2)
    print("String 3: ", string3)
end

local function string_manip()
    print("------------string_manip------------")

    print("Upper :", string.upper("abc"))
    print("Lower :", string.lower("ABC"))
    print("Orginal string: Lua Tutorial")
    print("Lua Tutorial => ", string.gsub("Lua Tutorial", "Tutorial", "Language"))
    print("Find: ", string.find("Lua Tutorial", "Tutorial"))
    local reversed = string.reverse("Lua Tutorial")
    print("Reversed: ", reversed)
end

local function string_format()
    print("------------string_format------------")

    local string1 = "Lua"
    local string2 = "Tutorial"

    local n1 = 10
    local n2 = 20

    print(string.format("Basic formatting %s %s", string1, string2))

    local date = 2; local month = 1; local year = 2014
    print(string.format("Date formatting %02d/%02d/%03d", date, month, year))

    print(string.format("%.4f", 1/3))
end

local function chars_and_bytes()
    print("------------chars_and_bytes------------")
    print(string.byte("Lua"))
    print(string.byte("Lua", 3))
    print(string.byte("Lua", -1))
    print(string.byte("Lua", 2))
    print(string.byte("Lua", -2))
    print(string.char(97))
    print(string.char(0x2b))
    print(string.char(241))
    print(string.len("This is a string"))
    print(string.rep("The", 3))
end

local function arrays()
    print("------------arrays------------")
    -- this will print "nil,Lua,Tutorial".  Indexing starts at 1
    -- the 0th element will be "nil"
    local array = {"Lua", "Tutorial"}
    for i = 0, 2 do
        print(array[i])
    end

    -- Arrays can have negative indices
    print("------------arrays - neg indicies------------")

    local array2 = {}
    for i = -2, 2 do 
        array2[i] = i * 2
    end

    for i = -2,2 do
        print(array2[i])
    end
end

local function multi_dim_arrays()
    print("------------multi_dim_arrays------------")
    local array = {}
    for i=1,3 do
        array[i] = {}

        for j=1,3 do
            array[i][j] = i*j
        end
    end

    print("------------multi-dim access------------")
    print(array)
    for i=1,3 do
        for j=1,3 do
            print("Array["..i.."]["..j.."]", array[i][j])
        end
    end

    print("------------multi-dim using manipulating indicies------------")
    local array = {}
    local maxrows = 3
    local maxcols = 3

    for row=1,maxrows do
        for col=1,maxcols do
            array[row*maxcols + col] = row*col
        end
    end

    print("------------multi-dim manip indicies access------------")
    for row=1,maxrows do
        for col=1,maxcols do
            print("Array["..row*maxcols+col.."]", array[row*maxcols + col])
        end
    end
end

local function iterators()
    print("------------iterators------------")
    local array = {"Lua", "Tutorial"}

    print("------------ipairs------------")
    for key,value in ipairs(array) do
        print(key, value)
    end
end

local function stateless(iteratorMaxCount, currentNumber)
    if currentNumber < iteratorMaxCount then
        currentNumber = currentNumber + 1
        return currentNumber, currentNumber*currentNumber
    end
end

local function squares(iteratorMaxCount)
    return stateless,iteratorMaxCount,0
end

local function stateful(collection)
    local index = 0
    local count = #collection

    -- closure function is returned
    return function()
        index = index + 1

        if index <= count then
            -- return current element
            return collection[index]
        end
    end
    --[[
        In the above example, we can see that stateful(collection) has another method 
        inside that uses the local external variables index and count to return 
        each of the element in the collection by incrementing the index each 
        time the function is called.

        We can create our own function iterators using closure as shown above 
        and it can return multiple elements for each of the time we iterate 
        through the collection.
    ]]
end

local function tables()
    local mytable = {}
    print("Type of mytable is ", type(mytable))

    mytable[1] = "Lua"
    mytable["wow"] = "Tutorial"

    print("mytable element at idx 1 is ", mytable[1])
    print("mytable element at idx wow is ", mytable["wow"])

    -- alternate table and mytable same memory/table
    local alternatetable = mytable

    print("alternatetable element at idx 1 is ", alternatetable[1])
    print("alternatetable element at idx wow is ", alternatetable["wow"])

    alternatetable["wow"] = "I changed it"

    print("mytable element at index wow (changed by alternatetable) is ", mytable["wow"])

    alternatetable = nil
    print("alternatetable is ", alternatetable)

    print("mytable element at index wow is ", mytable["wow"])
    mytable = nil
    print("mytable is ", mytable)
end

local function table_concat()
    local fruits = {"banana", "orange", "apple"}
    print("Original table ")
    for k,v in ipairs(fruits) do print(k,v) end

    print("Concatenated string ", table.concat(fruits))

    print("Concatenate with char ", table.concat(fruits, ", "))

    print("Concatenate string on idx ", table.concat(fruits, ", ", 2, 3))
end

local function table_ins_rem()
    local fruits = {"banana", "orange", "apple"}
    print("Original table ")
    for k,v in ipairs(fruits) do print(k,v) end

    table.insert(fruits, "mango")
    print("Fruit @ idx 4 ", fruits[4])

    table.insert(fruits,2,"grapes")
    print("Fruit @ idx 2 (insert) is ", fruits[2])

    print("Max elements is ", table.maxn(fruits))
    print("Last element is ", fruits[#fruits])

    table.remove(fruits)
    print("The previous last element is ", fruits[5])
end

local function table_sort()
    local fruits = {"banana", "orange", "apple"}
    print("Original table ")
    for k,v in ipairs(fruits) do print(k,v) end

    table.sort(fruits)
    print("Sorted table")
    for k,v in ipairs(fruits) do print(k,v) end
end

local function metatables()
    local mytable = setmetatable({key1 = "value1"}, {
        __index = function(mytable, key)
            if key == "key2" then
                return "metatablevalue"
            else
                return nil
            end
        end
    })
    print(mytable.key1, mytable.key2)

    -- reset it - compact version of above.
    mytable = {}
    local mytable = setmetatable({key1 = "value1"}, 
        {__index = { key2 = "metatablevalue" } })
    print(mytable.key1, mytable.key2)


end

local function main()
    basic()
    types()
    strings()
    string_manip()
    string_format()
    chars_and_bytes()
    arrays()
    multi_dim_arrays()
    iterators()

    print("------------stateless iterators------------")
    for i,n in stateless,3,0 do
        print(i, n)
    end

    print("------------stateless with sig like ipairs------------")
    for i,n in squares(3) do
        print(i, n)
    end

    print("------------stateful iterator------------")
    local array = {"Lua", "Tutorial", "Foobar"}
    for element in stateful(array) do
        print(element)
    end

    print("------------tables------------")
    tables()
    print("------------table concats------------")
    table_concat()
    print("------------table inserts/removes------------")
    table_ins_rem()
    print("------------table sort------------")
    table_sort()

    print("------------modules------------")
    print("See comments in this file for module information")

    print("------------metatables!------------")
    --local mytable = {}
    --local mymetatable = {}
    --setmetatable(mytable, mymetatable)
    --OR--
    --mytable = setmetatable({},{})
    metatables()
end

main()
