(* This program will need to do the following things. 
 *
 * 1. Read in instructions from an input file.
 * 2. Line by line convert each instruct to its repective int elements. 
 * 3. Pack the int elements in a resulting int 
 * 4. Print the resulting int to the command line, or to a specified output file. 
 *)


(* just for debugging *)
let printIntElements lst = 
    
    let helper str intopt = 
        match intopt with 
        | None -> "none " ^ str
        | Some x -> (Pervasives.string_of_int x) ^ " " ^ str 
    
    in List.fold_left helper "" lst
;;

(* determineOppcode 
 * string -> int 
 *
 * this function takes in an oppcode string
 * and returns it's associated int value.*)
let determineOppcode str = 
    let str = String.lowercase_ascii str in  
    match str with 
    | "add"  -> 0
    | "nand" -> 1
    | "lw"   -> 2
    | "sw"   -> 3
    | "beq"  -> 4
    | "jalr" -> 5
    | "noop" -> 6
    | "halt" -> 7
    | ".fill"-> 8
    | _      -> -1
;;


(* instruction Parser
 * string list -> string list list
 *
 *
 * This function takes in a list of assembly instructions
 * and splits each instruction into it's unique elements.
 * Will only split into a max of 6 elements,
 * [ tag; oppcode; field1; field2; field3; comments] *)
let instructParser instructs = 
    
    let helper line = 
        Str.bounded_split_delim (Str.regexp "[ \n\r\x0c\t]+") line 6;
    in 
    List.map helper instructs;  
;;


(* to Int 
 * string list -> int option list
 *
 * This function takes a list of assembly instructions
 * as strings, and will convert them to their respective int values*)
let toInt lineNumber instruction = 

    (* This helper will take an the element index and
     * the element from the instruction, and return either some 
     * of the appropriate int value or 
     * None if the element is a tag, or 
     * is a comment. *)
    let helper elemNum elem =
        match elemNum with 
        | 0 -> None (* Is a tag *)
       
        | 1 -> Some(determineOppcode elem) (* is a oppcode *)
       
        (* translate fields 1 and 2 to int *) 
        | 2 
        | 3 
        | 4 -> Some( int_of_string elem) 
        | _ -> None 
    
    in 
    List.rev (List.mapi helper instruction) 
    (* note.. maybe I can find a way so 
     * I dont need to reverse the list *)
;;


(* tag oppcode destR reg1 reg2 comments *)
(* packInt 
 * int option list -> int 
 *
 * this function takes in a list a of instruct elements 
 * and will returned the correctly packed int. 
 * This function uses mutation on an int refrence 
 * to calculate the packed int*)
let packInt instruct =

    let instructIntOpts = List.filter (fun x -> x != None) instruct in 
    let instructInts = List.rev_map (fun x -> 
        match x with 
        | None -> failwith "there should be no none value in the list" 
        | Some y -> y ) instructIntOpts 
    in 
    
    let result = ref 0x0 in  
    let helper elemNum elem  = 
        match elemNum with 
        | 0 -> result := !result lor ( elem lsl 22);
        | 1 -> result := !result lor ( elem lsl 19) 
        | 2 -> result := !result lor ( elem lsl 16)
        | 3 -> result := !result lor ( elem land 0x0000ffff) 
        | _ -> () 
    in List.iteri helper instructInts;
    
    if List.hd instructInts = 8
        (* if the oppcode is fill then 
            * just return the second element*)
        then List.hd (List.tl instructInts) 
    else 
        !result;
;;




(* Getopt stuff *)

let input  = ref "";;
let output = ref "";;

(* Specifies the available command line options
 * and their handlers.
 * In this case input and output can only have one argument *)
let specs  = 
[
    ('i', "input",  None,  (Getopt.atmost_once input  (Getopt.Error "Only one input allowed")  ));
    ('o', "output", None,  (Getopt.atmost_once output (Getopt.Error "Only one output allowed") )) ;
]



(* Main Function *)
let _ = 
   Getopt.parse_cmdline specs print_endline; 
   Printf.printf "input = %s\n" !input;
   Printf.printf "output = %s\n" !output;
  
  
   let inputList = Util.strlist_from_file !input in 
   let instruct_elements = instructParser inputList in 
   List.iter( fun x -> Printf.printf "%s \n" x ) inputList;

   let instruct_ints = List.mapi (fun x -> toInt x) instruct_elements in 
   List.iter( fun x -> 
       let str =  printIntElements  x in 
       Printf.printf "%s\n" str ) instruct_ints;
  
   List.iter (fun x ->
       let packed = packInt x in 
       Printf.printf "%d\n" packed ) instruct_ints; 
;;
