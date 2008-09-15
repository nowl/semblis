#ifndef __IAMARDOCS_H__
#define __IAMARDOCS_H__

//doc:start
//game semblis core
//  (display 'object')
//
//Brief Description: prints/shows a data object to the user
//
//domain : semblis
//
//explained: 
//  Outputs a string/number/list/variable/etc. to the user using whatever
//  printing callback function is enabled. This may print to the
//  console or to a window in Iamar depending on how it is configured.
//
//
//returns:
//	this function returns nothing
//
//example:
//  (define a "this is a test") 
//  (display a)
//		
//doc:end


//doc:start
//game semblis core
//  (newline)
//
//Brief Description: simply outputs a new line
//
//domain : semblis
//
//explained: 
//  Usually used in combination with the (display) function in order
//  to output a new line to the user.
//
//
//returns:
//	this function returns nothing
//
//example:
//  (newline)    ; a new line is output..
//		
//doc:end


//doc:start
//game semblis core
//  (define 'variable or (function with args)' 'expression or body')
//
//Brief Description: defines a new function or variable
//
//domain : semblis
//
//explained: 
//  Creates a new variable or function in the environment. 
//
//
//returns:
//	this function returns nothing
//
//example:
//  (define a "this is a test")    ; a is a string
//  (define (plus a b) (+ a b))    ; plus is a function
//		
//doc:end


//doc:start
//game semblis core
//  (lambda '(formal variable)' 'body')
//
//  Brief Description: create a function object
//
//domain : semblis
//
//explained: 
//  Create a function taking the various variables
//  as formal parameters and executes the code in body.
//
//returns:
//	this function returns nothing
//
//example:
//  (define plus (lambda (a b) (+ a b)))
//  (display (plus 1 3))    ; outputs a 4
//		
//doc:end


//doc:start
//game semblis core
//  (if 'test' 'true-cond' 'false-condition')
//
//  Brief Description: Conditional if statement
//
//domain : semblis
//
//explained: 
//  The expression 'test' is evaluated. If it evaluates to true then
//  the 'true-cond' is then evaluated, if false then the
//  'false-condition' is evaluated.
//
//returns:
//	this function returns nothing
//
//example:
//  (if (= 1 2)
//    (display 5)
//    (display 6))   ; displays a 6
//		
//doc:end

//doc:start
//game semblis core
//  (cond (('test1') 'result1')
//        (('test2') 'result2')
//        ....
//        (t 'default-result'))
//
//  Brief Description: Conditional statement
//
//domain : semblis
//
//explained: 
//  The expression 'test1' is evaluated. If it evaluates to true then
//  the 'result1' is then evaluated, if false then the 'test2' is
//  evaluated and if true then 'result2' is evaluated and so on. If
//  all tests are false then the 'default-result' is evaluated.
//
//returns:
//	this function returns nothing
//
//example:
//  (cond ((= 1 2) (display 3))
//        ((= 5 5) (display 5))
//        (t (display 6)))    ; displays a 5
//		
//doc:end

//doc:start
//game semblis core
//  (or 'expressions..')
//
//  Brief Description: or's expressions
//
//domain : semblis
//
//explained: 
//  Each expression passed into this function is evaluated until one
//  is true in which case entire function returns true and the
//  remaining expressions are left un-evaluated.
//
//returns:
//	this function returns the value that was true
//
//example:
//  (or (+ 1 2) (+ 5 6))   ; returns 3
//		
//doc:end


//doc:start
//game semblis core
//  (and 'expressions..')
//
//  Brief Description: and's expressions
//
//domain : semblis
//
//explained: 
//  Each expression passed into this function is evaluated until one
//  is false in which case entire function returns false and the
//  remaining expressions are left un-evaluated.
//
//returns:
//	this function returns true or false
//
//example:
//  (and (+ 1 2) (+ 5 6))   ; returns true
//		
//doc:end


//doc:start
//game semblis core
//  (let '(('var' 'exp') ('var exp') ...) 'body')
//
//  Brief Description: defines some locally scoped variables
//
//domain : semblis
//
//explained: 
//  Assigns each variable 'var' to the expression 'exp' for use as
//  local variables in the 'body'.
//
//returns:
//  result of the last evaluation in the body
//
//example:
//	(let ((a 1)
//        (b 2))
//    (+ a b))   ; returns 3
//
//doc:end


//doc:start
//game semblis core
//  (str 'pointer')
//
//  Brief Description: convert a pointer to a semblis string
//
//domain : semblis
//
//explained: 
//  Pass in a pointer (integer) and returns a semblis string
//  object. This is useful in foreign function which return character
//  pointers and you want to work with them in semblis as actual
//  semblis strings.
//
//returns:
//  String object.
//
//example:
//  (define char-pointer (some-func-returning-pointer))
//  (define s (str char-pointer))
//
//doc:end

//doc:start
//game semblis core
//  (ptr 'string object')
//
//  Brief Description: converts a semblis string to a pointer
//
//domain : semblis
//
//explained: 
//  Takes a string object and returns a pointer. This is useful for
//  foreign functions taking pointers to strings as arguments.
//
//returns:
//  pointer to the string
//
//example:
//	(define pointer (ptr "this is a test"))
//  (function-that-takes-pointer pointer)
//doc:end

//doc:start
//game semblis core
//  (first 'list')
//
//  Brief Description: returns first element from a list
//
//domain : semblis
//
//explained: 
//  Useful for getting at the first element of a basic list or data
//  object. Same as 'car' in scheme/lisp.
//
//returns:
//  data object
//
//example:
//  (define first-element (list 'a 'b 'c))  ; first-element is "a"
//		
//doc:end

//doc:start
//game semblis core
//  (first 'rest')
//
//  Brief Description: returns remaining elements from a list
//
//domain : semblis
//
//explained: 
//  Useful for getting at the rest of the elements of a basic list or
//  data object. Same as 'cdr' in scheme/lisp.
//
//returns:
//  list
//
//example:
//  (define rest-elements (rest 'a 'b 'c))  ; first-element is '(b c)
//		
//doc:end

//doc:start
//game semblis core
//  (list 'list of elements')
//
//  Brief Description: creates a new list out of primitives
//
//domain : semblis
//
//explained: 
//  This creates a new list out of the arguments passed in.
//
//returns:
//  a list
//
//example:
//  (list 'a 'b "test") ; results in the list ('a 'b "test")		
//
//doc:end


//doc:start
//game semblis core
//  (push 'element' 'list')
//
//  Brief Description: adds an element to a list
//
//domain : semblis
//
//explained: 
//  Adds the element 'element' to the front of the list 'list'. It
//  does not modify the list but instead returns a new list. Same as
//  'cons' in scheme/lisp.
//
//returns:
//  a new list
//
//example:
//  (push 'a '(b c d)) ; results in '(a b c d)		
//
//doc:end

//doc:start
//game semblis core
//  (import 'file')
//
//  Brief Description: import a semblis file
//
//domain : semblis
//
//explained: 
//  Reads in and evaluates another file. This is useful for including
//  other code within the same semblis file such as library functions.
//
//returns:
//  nothing
//
//example:
//	(import "utils.sem")
//
//doc:end

//doc:start
//game semblis core
//  (eval 'expression')
//
//  Brief Description: evaluates an expression
//
//domain : semblis
//
//explained: 
//  Runs the evaluator on the expression, symbol, or data
//  object. Returns the result of that evaluation.
//
//returns:
//  Evaluation result
//
//example:
//  (eval '(+ 1 2))   ; returns 3
//		
//doc:end


//doc:start
//game semblis core
//  (set 'variable' 'expression')
//
//  Brief Description: sets a variable to a value
//
//domain : semblis
//
//explained: 
//  Assigns the result of 'expression' to the given 'variable'. This
//  is similar to define except it does not create a new variable,
//  instead it acts on variables already created using define.
//
//returns:
//  nothing
//
//example:
//  (define a 5)
//  (set a 7)   ; 'a' now equals 7		
//
//doc:end

//doc:start
//game semblis core
//  (while ('expression') 'body')
//
//  Brief Description: looping construct
//
//domain : semblis
//
//explained: 
//  Continues to evaluate 'body' over and over until 'expression'
//  evaluates to false.
//
//returns:
//  nothing
//
//example:
//  (define a 3)
//  (while (> a 1)
//    (display a)
//    (set a (- a 1)))		
//
//doc:end

//doc:start
//game semblis core
//  (not 'expression')
//
//  Brief Description: negates the result of an expression
//
//domain : semblis
//
//explained: 
//  Evaluates expression and returns the opposite value of what it
//  evaluates to, either true or false (nil).
//
//returns:
//  true or nil
//
//example:
//  (not 5)   ; evaluates to nil		
//
//doc:end


//doc:start
//game semblis core
//  (= 'expression1' 'expression2')
//
//  Brief Description: returns the result of two expressions being equal
//
//domain : semblis
//
//explained: 
//  Both expressions are evaluated, if they are both the same then the
//  result is true. Otherwise the result is nil.
//
//returns:
//  true or nil
//
//example:
//	(= 2 (+ 1 1))   ; evaluates to true
//doc:end


//doc:start
//game semblis math
//  (+ 'numbers')
//
//  Brief Description: adds numbers
//
//domain : semblis
//
//explained: 
//  Adds up the values represented by numbers and returns the
//  resulting addition.
//
//returns:
//  a number
//
//example:
//  (+ 1 5 7 (* 2 2))    ; evaluates to 17		
//
//doc:end


//doc:start
//game semblis math
//  (- 'numbers')
//
//  Brief Description: subtracts numbers
//
//domain : semblis
//
//explained: 
//  Subtracts all subsequent numbers from the first number in the
//  'numbers' list.
//
//returns:
//  a number
//
//example:
//  (- 10 5 4)   ; evaluates to 1
//		
//doc:end


//doc:start
//game semblis math
//  (* 'numbers')
//
//  Brief Description: multiplies numbers
//
//domain : semblis
//
//explained: 
//  Multiplies the the values represented by numbers and returns the
//  resulting multiplication.
//
//returns:
//  a number
//
//example:
//  (* 2 3 4)    ; evaluates to 24
//		
//doc:end


//doc:start
//game semblis math
//  (/ 'numbers')
//
//  Brief Description: divides numbers
//
//domain : semblis
//
//explained: 
//  Divides the first value by all numbers represented by numbers and
//  returns the result.
//
//returns:
//  a number
//
//example:
//  (/ 24 2 6)    ; evaluates to 2
//		
//doc:end


//doc:start
//game semblis math
//  (< 'number1' 'number2')
//
//  Brief Description: check for less than
//
//domain : semblis
//
//explained: 
//  Returns true if 'number1' is less than 'number2', otherwise
//  returns nil.
//
//returns:
//  true or nil
//
//example:
//  (< 4 4)    ; evaluates to nil
//		
//doc:end


//doc:start
//game semblis math
//  (> 'number1' 'number2')
//
//  Brief Description: check for greater than
//
//domain : semblis
//
//explained: 
//  Returns true if 'number1' is greater than 'number2', otherwise
//  returns nil.
//
//returns:
//  true or nil
//
//example:
//  (> 5 4)    ; evaluates to true
//		
//doc:end


//doc:start
//game semblis math
//  (<= 'number1' 'number2')
//
//  Brief Description: check for less than or equal
//
//domain : semblis
//
//explained: 
//  Returns true if 'number1' is less than or equal to 'number2',
//  otherwise returns nil.
//
//returns:
//  true or nil
//
//example:
//  (<= 4 4)    ; evaluates to true
//		
//doc:end


//doc:start
//game semblis math
//  (>= 'number1' 'number2')
//
//  Brief Description: check for greater than or equal
//
//domain : semblis
//
//explained: 
//  Returns true if 'number1' is greater than or equal to 'number2',
//  otherwise returns nil.
//
//returns:
//  true or nil
//
//example:
//  (>= 5 4)    ; evaluates to true
//		
//doc:end

//doc:start
//game semblis math
//  (random)
//
//  Brief Description: returns a random number
//
//domain : semblis
//
//explained: 
//  Generates a random real number between 0 and 1.
//
//returns:
//  a number
//
//example:
//  (define random-number (random))
//		
//doc:end

//doc:start
//game semblis math
//  (mod 'number1' 'number2')
//
//  Brief Description: modulo operation
//
//domain : semblis
//
//explained: 
//  Returns 'number1' modulo 'number2'.
//
//returns:
//  a number
//
//example:
//  (mod 5 2)    ; evaluates to 1
//		
//doc:end


//doc:start
//game semblis math
//  (int 'number1')
//
//  Brief Description: cast to integer
//
//domain : semblis
//
//explained: 
//  Casts the given number 'number1' to an integer.
//
//returns:
//  a number
//
//example:
//  (int 4.2)  ; evaluates to 4
//		
//doc:end

#endif  /* __IAMARDOCS_H__ */
