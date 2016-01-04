License
=========

Copyright (C) 2016 Jianxin Wang(jxwang@mail.csu.edu.cn), Zhongxiang Liao(liaozx@csu.edu.cn)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.

Jianxin Wang(jxwang@mail.csu.edu.cn), Zhongxiang Liao(liaozx@csu.edu.cn)
School of Information Science and Engineering
Central South University
ChangSha
CHINA, 410083


ISEA   
=========
ISEA is a de nove assembly tool used for paired-read libraries of short reads. To get better performance, you'd better provide a mate pair library.
       
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
a. configure file "config"     
   there is a example config file in ISEA's installation directory, you can copy this file to your working directory and modify it. There are some comments in this config file, which can help you to use it.      
       
b. type command     
	perl your_ISEA's_director/bin/isea [-fast]     
   the option -fast will only influence the error correction step, and do not influence the assembly result, if your memory is enough, you can specify -fast.    
  
3)Output.
combine.fasta contains resulting contigs
scaffold.fasta contains resulting scaffolds
