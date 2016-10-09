#!/bin/sh


#Function Name: backend_statistics
#Argument: objdump.file basic_block.file
#This function statistics instructions's appearance

function  backend_statistics()
{
	addr_start=0
	addr_end=0
	num=0
	addr_insn=0
	insn=0

	cut -f 1,3 $1 | grep "  .*:"| sed 's/://g' > assemble_txt

	grep -v "[[:space:]]0$" $2 > bb_counter_txt

	:>result_txt

#	echo "3"

	while true
	do
		if ! read addr_start addr_end num <&4
		then
			break	
		fi

		while [ $addr_start != $addr_insn ]
		do
			read addr_insn insn <&3
		done
	
		while [ $addr_insn \< $addr_end ]
		do
			echo -e "$insn\t$num" >> result_txt
		
			read addr_insn insn <&3
		done

	done 3<assemble_txt 4<bb_counter_txt

	sort result_txt > _result_txt
	#cp _result.txt hello.txt
	mv _result_txt result_txt

#	echo "5"

	head -1 result_txt > t_txt
	read insn_pre num_sum < t_txt
	num_sum=0

	while read insn num 
	do
		if [ $insn_pre = $insn ]
		then
			num_sum=`expr $num_sum + $num` 
		else
			echo -e "$insn_pre\t$num_sum" 

			insn_pre=$insn
			num_sum=$num
		fi	
	done < result_txt
#	echo "6"
	echo -e "$insn_pre\t$num_sum" 
	rm result_txt  t_txt bb_counter_txt assemble_txt
}



$*
objdump -d  $1 > objdump.file
backend_statistics objdump.file  bb_counter.txt > i_txt
sort -k 2 -n i_txt >  instructions.txt 
#rm objdump.file bb_counter.txt i_txt

