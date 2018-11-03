(* This program will need to do the following things. 
 *
 * 1. Read in instructions from an input file.
 * 2. Line by line convert each instruct to its repective int elements. 
 * 3. Pack the int elements in a resulting int 
 * 4. Print the resulting int to the command line, or to a specified output file. 
 *)


let instruction_parser line = 
    let strlist =
        Str.split (Str.regexp "[ \n\r\t]+");
    in strlist 
;;


let input  = ref "";;
let output = ref "";;

(* specifies the available command line options
 * and their handlers.
 * In this case input and output can only have one argument *)
let specs  = 
[
    ('i', "input",  None,  (Getopt.atmost_once input  (invalid_arg "Only one input allowed")  ) );
    ('o', "output", None,  (Getopt.atmost_once output (invalid_arg "Only one output allowed") ) ) ;
]



(* Main Function *)
let _ = 
   Getopt.parse_cmdline specs print_endline; 
   Printf.printf "input = %s\n" !input;
   Printf.printf "output = %s\n" !output;

