#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdlib.h>



#define XSTR(x) #x
#define STR(x) XSTR(x)



using namespace std;



void help(){
	cout<<"BLIAL"<<endl
	<<"Options and default values: "<<endl
	<<"-x for paired read file"<<endl
	<<"-u for unpaired read file"<<endl
	<<"-o for working folder (.)"<<endl
	<<"-s for kmer solidity threshold (2)"<<endl
	<<"-S for unitig solidity threshold (2)"<<endl
	<<"-K for largest kmer size (100)"<<endl
	<<"-k for first kmer size (100)"<<endl
	<<"-r for ecart between kmer size"<<endl
	<<"-t for core used (max)"<<endl
	;
}



int main(int argc, char *argv[]) {
	if(argc<2){
		help();
		exit(0);
	}
	string pairedFile(""),unPairedFile(""),workingFolder("."),prefixCommand(""),folderStr(STR(folder)),bgreatArg;
	uint kMax(100),kMin(21),solidity(2),coreUsed(0),unitigFilter(2),kRate(10);//lol
	if(folderStr!=""){
		prefixCommand=folderStr+"/";
	}
	char c;
	while ((c = getopt (argc, argv, "u:x:o:s:k:K:p:c:t:S:r:")) != -1){
	switch(c){
		case 'u':
			unPairedFile=realpath(optarg,NULL);
			break;
		case 'x':
			pairedFile=realpath(optarg,NULL);
			break;
		case 'o':
			workingFolder=(optarg);
			break;
		case 's':
			solidity=stoi(optarg);
			break;
		case 'S':
			unitigFilter=stoi(optarg);
			break;
		case 'r':
			kRate=stoi(optarg);
			break;
		case 'k':
			kMin=stoi(optarg);
			break;
		case 'K':
			kMax=stoi(optarg);
			break;
		case 't':
			coreUsed=stoi(optarg);
			break;
		}
	}
	if(pairedFile=="" and unPairedFile==""){
		help();
		exit(0);
	}
	c=system(("mkdir "+workingFolder).c_str());
	c=chdir(workingFolder.c_str());
	c=system("mkdir logs");
	ofstream param("ParametersUsed.txt");
	ofstream bankBcalm("bankBcalm.txt");
	param<<"kmax: "<<kMax<<" solidity: "<<solidity<<" unitig solidity: "<<unitigFilter<<endl;
	uint filesCase(0);
	if(pairedFile==""){
		filesCase=1;
		bgreatArg=" -u "+unPairedFile;
		bankBcalm<<unPairedFile<<endl;
	}else{
		if(unPairedFile==""){
			filesCase=2;
			bgreatArg=" -x "+pairedFile;
			bankBcalm<<pairedFile<<endl;
		}else{
			filesCase=3;
			bankBcalm<<pairedFile<<endl<<unPairedFile<<endl;
			bgreatArg=" -x"+pairedFile+"  -u "+unPairedFile+" ";
		}
	}
	cout<<filesCase<<endl;



	string fileBcalm("bankBcalm.txt");
	uint kmerSize(kMin);
	while(kmerSize<kMax){
		cout<<"Graph construction "+to_string(kmerSize)<<endl;
		c=system((prefixCommand+"bcalm -in "+fileBcalm+" -kmer-size "+to_string(kmerSize)+" -abundance-min "+to_string(solidity)+" -out out  -nb-cores "+to_string(coreUsed)+"  >>logs/logBcalm 2>>logs/logBcalm").c_str());
		c=system((prefixCommand+"kMILL out.unitigs.fa $(("+to_string(kmerSize)+"-1)) $(("+to_string(kmerSize)+"-2)) >>logs/logBcalm 2>>logs/logBcalm").c_str());
		c=system(("mv out_out.unitigs.fa.fa dbg"+to_string(kmerSize)+".fa").c_str());
		cout<<"Read mapping on the graph "+to_string(kmerSize)<<endl;
		c=system((prefixCommand+"bgreat -k "+to_string(kmerSize)+" "+bgreatArg+" -g dbg"+to_string(kmerSize)+".fa -t "+to_string((coreUsed==0)?10:coreUsed) +"  -m 0 -e 1 >>logs/logBgreat 2>>logs/logBgreat").c_str());
		c=system((prefixCommand+"numbersFilter paths "+to_string(unitigFilter)+" > cleanedPaths 2>>logs/logBgreat").c_str());
		c=system((prefixCommand+"numbersToSequences dbg"+to_string(kmerSize)+".fa  cleanedPaths  $(("+to_string(kmerSize)+"-1)) >newPaths 2>>logs/logBgreat").c_str());
		fileBcalm="newPaths";
		kmerSize+=kRate;
		solidity=1;
	}
	c=system(("rm bankBcalm.txt newPaths notAligned.fa out.h5 out.unitigs.fa paths"));
	cout<<"The end"<<endl;

    return 0;
}
