#!/bin/bash                                                                                
                                                                                                       
while true; do                                                                             
    echo dd iteration $(( ++i ))                                                           
    ( for ((j=0 ; j < 32 ; j++)); do                                                       
    	dd of=/dev/null if=file$j bs=1K 2>&1 >/dev/null &                                  
    done                                                                                   
    wait                                                                                   
    ) > /dev/null                                                                          
done               
