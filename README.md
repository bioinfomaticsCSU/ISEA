ISEA is an de nove assembly tool used for paired-read libraries of short reads. To get better performance, you'd better provide a mate pair library.
       
1)Installing.      
Requirements:   
g++    
perl    
python    

type command:    
tar zxvf ISEA.tar.gz     
cd your_ISEA's_director/src     
make    
     
2)Runing.    
1. configure file "config"    
   there is a example config file in ISEA's installation directory, you can copy this to your working directory and modify it. There are some comments in this config file, which can help you to use it.
     
2. type command     
	perl your_ISEA's_director/bin/isea [-fast]    
   the option -fast will only influence the error correction step, and do not influence the assembly result, if your memory is enough, you can specify -fast.
    
