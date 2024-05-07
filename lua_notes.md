# Lua Notes

Little snippets of lua wisdom that I see floating around the web that are useful (or better explanations than what's in the official book / reference)

## Lua explanations
<details>
<summary>The ellipsis operator ... : Variadics - how do they work?</summary>
In response to this question:

```
I recently read about lua and addons for the game "World of Warcraft". Since the interface language for addons is lua and I want to learn a new language, I thought this was a good idea.

But there is this one thing I can't get to know. In almost every addon there is this line on the top which looks for me like a constructor that creates a object on which member I can have access to. This line goes something like this:

object = {...}
I know that if a function returns several values (which is IMHO one huge plus for lua) and I don't want to store them seperatly in several values, I can just write

myArray = {SomeFunction()}
where myArray is now a table that contains the values and I can access the values by indexing it (myArray[4]). Since the elements are not explicitly typed because only the values themselfe hold their type, this is fine for lua. I also know that "..." can be used for a parameter array in a function for the case that the function does not know how many parameter it gets when called (like String[] args in java). But what in gods name is this "curly bracket - dot, dot, dot - curly bracket" used for???
```

This answer:
```
You've already said all there is to it in your question:

{...} is really just a combination of the two behaviors you described: It creates a table containing all the arguments, so

function foo(a, b, ...)
   return {...}
end

foo(1, 2, 3, 4, 5) --> {3, 4, 5}
Basically, ... is just a normal expression, just like a function call that returns multiple values. The following two expressions work in the exact same way:

local a, b, c = ...
local d, e, f = some_function()
Keep in mind though that this has some performance implications, so maybe don't use it in a function that gets called like 1000 times a second ;)

EDIT:

Note that this really doesn't apply just to "functions". Functions are actually more of a syntax feature than anything else. Under the hood, Lua only knows of chunks, which are what both functions and .lua files get turned into. So, if you run a Lua script, the entire script gets turned into a chunk and is therefore no different than a function.

In terms of code, the difference is that with a function you can specify names for its arguments outside of its code, whereas with a file you're already at the outermost level of code; there's no "outside" a file.

Luckily, all Lua files, when they're loaded as a chunk, are automatically variadic, meaning they get the ... to access their argument list.

When you call a file like lua script.lua foo bar, inside script.lua, ... will actually contain the two arguments "foo" and "bar", so that's also a convenient way to access arguments when using Lua for standalone scripts.

In your example, it's actually quite similar. Most likely, somewhere else your script gets loaded with load(), which returns a function that you can call—and, you guessed it, pass arguments to.

Imagine the following situation:

function foo(a, b)
   print(b)
   print(a)
end

foo('hello', 'world')
This is almost equivalent to

function foo(...)
   local a, b = ...
   print(b)
   print(a)
end

foo('hello', 'world')
Which is 100% (Except maybe in performance) equivalent to

-- Note that [[ string ]] is just a convenient syntax for multiline "strings"
foo = load([[
   local a, b = ...
   print(b)
   print(a)
]])

foo('hello', 'world')
```
</details>

<details>
<summary>String Splitting</summary>

See [here](https://stackoverflow.com/questions/1426954/split-string-in-lua) for various examples of string splitting

See [here](http://lua-users.org/wiki/SplitJoin) for the lua-users discussion on string splitting - various simple and advanced splitting functions.
</details>

<details>
<summary>Multi-Byte Splitting</summary>

See [here](https://stackoverflow.com/questions/9003747/splitting-a-multibyte-string-in-lua) for a decent example(s) of UTF-8 splitting
</details>

<details>
<summary>Byte array splitting</summary>

</details>