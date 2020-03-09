//============================================================================
// Name        : MarkovTweet.cpp
// Author      : Nicolas McDermott, Marina Smolens
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// Include This Stuff //
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <time.h>
#include <math.h>
using namespace std;



// ------ Hash Node Declarations ------ //
// ------------------------------------ //

class hashNode {
	friend class hashMap;
	string keyword;   // this is used as the key – each key is only inserted once!
	string *values;   // the dynamically allocated array (not linked list!) of words that follow the key in the Dr. Seuss text.  If this set of words gets to be larger than the valuesSize of the array, then you must re-allocate the array double in size and copy over all the old values
	int valuesSize;  // the size of the values array
	int currSize;  // the number of words in the values array so far
public:
	hashNode(); //constructor-initializes keyword to “” , valuesSize to 0, and currSize to 0 and values to NULL
	hashNode(string s);  // initializes keyword to s, the valuesSize to 100 (or whatever you like for starting), the currSize to 0, and the values to be a dynamically allocated array of valuesSize
	hashNode(string s, string v);  // in addition, puts a value in the values array and initializes currSize to 1
	void addValue(string v);  // adds a new value to the end of the values array, increases currSize, checks to make sure there’s more space, and, if not, calls dblArray()
	void dblArray(); //creates a new array, double the length, and copies over the values.  Sets the values array to be the newly allocated array.
	string getRandValue();  // returns a random string from the values array.  If there’s no values in the value array, then it returns an empty string.
	void printHashNode(); // for debugging
};

// ------ Hash Map Declarations ------ //
// ----------------------------------- //

class hashMap {
	friend class getTweet;
	hashNode **map;  //a single dimensional dynamically allocated array of pointers to hashNodes
	string first; // for first keyword for printing to a file
	int numKeys;
	int mapSize;
	bool h1; // if true, first hash function used, otherwise second hash function is used
	bool c1; //if true, first collision method used, otherwise second collision method is used.
	int collisionct1;  //count of original collisions (caused by the hashing function used)
	int collisionct2; //count of secondary collisions (caused by the collision handling method used)
	public:
		//ME
		hashMap(bool hash1, bool coll1);  // when creating the map, make sure you initialize the values to NULL so you know whether that index has a key in it or not already.  The Boolean values initialize the h1 and the c1 boolean values, making it easier to control which hash and which collision methods you use.
		void addKeyValue(string k, string v);
		// adds a node  to the map at the correct index based on the key string, and then inserts the value into the value field of the hashNode
		// Must check to see whether there's a node at that location.  If there's nothing there (it's NULL), add the hashNode with the keyword and value.
		// If the node has the same keyword, just add the value to the list of values.
		//If the node has a different keyword, keep calculating a new hash index until either the keyword matches the node at that index's keyword, or until the
		// map at that index is NULL, in which case you'll add the node there.
		//This method also checks for load, and if the load is over 70%, it calls the reHash method to create a new longer map array and rehash the values
		//if h1 is true, the first hash function is used, and if it’s false, the second is used.
		//if c1 is true, the first collision method is used, and if it’s false, the second is used
		int getIndex(string k); // uses calcHash and reHash to calculate and return the index of where the keyword k should be inserted into the map array
		int calcHash(string k);  // hash function
		int calcHash2(string k);  // hash function 2
		void getClosestPrime();  // I used a binary search on an array of prime numbers to find the closest prime to double the map Size, and then set mapSize to that new prime.  You can include as one of the fields an array of prime numbers, or you can write a function that calculates the next prime number.  Whichever you prefer.
		void reHash();  // when size of array is at 70%, double array size and rehash keys
		int collHash1(int h, int i, string k);  // getting index with collision method 1 (note – you may modify the parameters if you don’t need some/need more)
		int collHash2(int h, int i, string k);  // getting index with collision method 2 (note – you may modify the parameters if you don’t need some/need more)
		int findKey(string k);  //finds the key in the array and returns its index.  If it's not in the array, returns -1
		void printMap();  //I wrote this solely to check if everything was working.
};

// ------ makeTweet Declarations ------ //
// ------------------------------------ //

class getTweet {
	hashMap *ht;
	string fn;  // file name for input (“DrSeuss.txt”)
	string newfile;  // name of output file
public:
	getTweet(bool h1, bool c1);
	void readFile();
	void writeFile();
	bool fileCheck(const std::string& name);

};

// -------- hashNode Body -------- //
// ------------------------------- //

hashNode::hashNode(){
	keyword = "";
	currSize = 0;
	valuesSize = 0;
	values = NULL;
}

hashNode::hashNode(string s){
	keyword = s;
	currSize = 0;
	valuesSize = 100;
	values = new string[valuesSize];

}

hashNode::hashNode(string s, string v){
	keyword = s;
	currSize = 1;
	valuesSize = 100;
	values = new string[valuesSize];
	values[0] = v;
}
void hashNode::addValue(string v){
	values[currSize] = v;
	currSize ++;
	if(currSize == valuesSize){
		dblArray();
	}
}

void hashNode::dblArray(){
	string *tempValues = new string[2*valuesSize];
	for(int i = 0; i < valuesSize; i++){
		tempValues[i] = values[i];
	}
	valuesSize = 2*valuesSize;
	values = tempValues;

}
string hashNode::getRandValue(){
	if (currSize==0){
		return "";
	}
	else{
		return values[rand()%currSize];
	}
}

// -------- hashMap Body -------- //
// ------------------------------ //

int primeNumbers[1000] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53, // an array of prime numbers for fun
			59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,
			149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,
			233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,
			331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,
			431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,
			523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,
			631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,
			739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,
			853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,
			967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,
			1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,
			1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,
			1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,
			1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,
			1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,
			1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,
			1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,
			1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,
			1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,
			1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,
			2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,
			2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,
			2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,
			2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,
			2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,
			2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,
			2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,
			2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,
			2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,
			2939,2953,2957,2963,2969,2971,2999,3001,3011,3019,3023,3037,3041,
			3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,
			3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,
			3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,
			3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,
			3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,
			3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,
			3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,
			3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,
			4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,
			4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,
			4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,
			4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,
			4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,
			4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,
			4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,
			4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,
			4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,
			5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,
			5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,
			5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,
			5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,
			5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,
			5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,
			5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,
			5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,
			5881,5897,5903,5923,5927,5939,5953,5981,5987,6007,6011,6029,6037,
			6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,
			6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,
			6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,
			6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,
			6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,
			6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,
			6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,
			6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,
			6949,6959,6961,6967,6971,6977,6983,6991,6997,7001,7013,7019,7027,
			7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,
			7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,
			7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,
			7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,
			7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,
			7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,
			7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,
			7879,7883,7901,7907,7919};

hashMap::hashMap(bool hash1,bool coll1){
	first = "";
	numKeys = 0;
	mapSize = 27;
	h1 = hash1;
	c1 = coll1;
	collisionct1 = 0;
	collisionct2 = 0;

	map = new hashNode*[mapSize]; // initialize map by filling it with null nodes
	for(int i=0;i<mapSize;i++){
		map[i] = NULL;
	}
}

void hashMap::addKeyValue(string k,string v){
	int index = getIndex(k); // get an index

	if(map[index]==NULL){ // if the node at the index is empty
		map[index] = new hashNode(k,v); // create a new node
		numKeys++;
	}
	else{ // if the node at the index is the same key word
		map[index]->addValue(v); // add a value to that key words
	}

	if(numKeys>=0.7*mapSize){ // If the map is too filled, rehash
		reHash();
	}
}

int hashMap::calcHash(string k){
	// hash function
	int result = 0;
	for(int i = 0; i < k.length() && i < 3; i++) { // for the first 3 or however many characters there are
		result += k[i] * pow(31, i); // increment result by the string letter at place i multipilied by prime number 31 to the power of i
	}
	return result % mapSize;
}

int hashMap::calcHash2(string k){ // I got this hash function from the internet
	#define A 54059 /* a prime */
	#define B 76963 /* another prime */
	#define C 86969 /* yet another prime */
	#define FIRSTH 37 /* also prime */
	unsigned h = FIRSTH;
	int index= 0;
	while (index < 3) { // for first 3 characters in the string
	 h = (h * A) ^ (k[0] * B); // modify strings and use prime numbers to alter h
	 index++;
	}
	return h % mapSize; // return h modded by the mapsize to get an index within the mapsize
}

void hashMap::getClosestPrime(){
	// I used a binary search on an array of prime numbers to find the
	//closest prime to double the map Size, and then set mapSize to that
	//new prime.  You can include as one of the fields an array of prime
	//numbers, or you can write a function that calculates the next prime
	//number.  Whichever you prefer.
	int nextPrime;
	for ( int i = 0; i < 1000; i ++){
		if (primeNumbers[i] > (mapSize * 2)){ // returns the next prime greater than double the surrent map size
			nextPrime = primeNumbers[i];
			break;
		}
		else{
			nextPrime = primeNumbers[mapSize + 10]; // precaution
		}
	}
	mapSize = nextPrime; // update map size
}

void hashMap::reHash(){
	int sizeOG = mapSize; // keep track of initial map size
	int newIndex;

	getClosestPrime(); // update map size to next prime number

	hashNode **tmp = map; // placeholder to create new map

	map = new hashNode*[mapSize];
	for(int i=0;i<mapSize;i++){ // initialize nodes in map
		map[i] = NULL;
	}

	for(int i=0;i<sizeOG;i++){
		if(tmp[i]!=NULL){ // fill nodes in new map with nodes from old map (if node is empty)
			newIndex = getIndex(tmp[i]->keyword);
			map[newIndex] = tmp[i];
		}
	}
}

int hashMap::getIndex(string k){
	// uses calcHash and reHash to calculate and return the index
	// of where the keyword k should be inserted into the map array
	int index;
	int count = 0;
	if(h1 == true){ //if we are using hashing function 1
		index = calcHash(k);
	}

	else{ //if we are using hashing function 2
		index = calcHash2(k);
	}

	if(map[index]==NULL||map[index]->keyword==k){
			return index;
		}
	else{
		collisionct1++;
		while (map[index]!=NULL && map[index]->keyword!=k){
			collisionct2 += 1;
			if(c1){
				index = collHash1(index, count, k);
			}
			else{
				index = collHash2(index, count, k);
			}
			count++;
		}
		return index;
	}
}
// ME
int hashMap::collHash1(int h, int i, string k){ //Using array of prime numbers
	if (h >= (mapSize - 1)){ // prevent indexing past size of map
		return 0;
	}
	else{
		return (h+primeNumbers[i])%mapSize; // return index using array of prime nums
	}

}


// ME
int hashMap::collHash2(int h, int i, string k){ // Using quadtratic probing
	if (h >= (mapSize - 1)){ // prevent indexing past size of map
		return 0;
	}
	else{
		return (h+(i*i))%mapSize; // return next index (cubic probing)
	}
}

int hashMap::findKey(string k){
	for(int i=0;i<mapSize;i++){ // traverse map
		if(map[i]->keyword==k){ // if the keyword is present return the keyword
			return i;
		}
	}
	return -1; // if there is no keyword of string k return -1
}


// -------- makeTweet Body -------- //
// -------------------------------- //

// ------ File Check ------ //
// ------------------------ //


getTweet::getTweet(bool h1, bool c1) {
	ht = new hashMap(h1,c1);
	string rf, wf; // read file and write file names
	int run = 1;
	// Determine whether the user inputs an actual file name
	while (run){
		printf("Type in your file name to read in: \n");
		std::getline(std::cin, rf);
		if (fileCheck(rf)){
			printf("File name accepted.");
			run = 0;
		}
		else{
			printf("File name incorrect or missing, please enter a new file name.");
			run =1;
		}
	}
	printf("Type in a file name to write to:");
	std::getline(std::cin, wf);
	newfile = wf.append(".txt"); // file to write to
	fn = rf; // file to read
	readFile();
	writeFile();
}

void getTweet::readFile() {
	ifstream infile(fn.c_str(),ios::in);     // open file
	string key = "";
	string value= "";
	infile>> key;
	ht->first = key;
	while (infile >> value) {          // loop getting single characters
		cout << key <<": " << value << endl;
		ht->addKeyValue(key,value);
		key = value;
		value = "";
//		cout << "map size: " << ht->mapSize << endl;
//		cout << "# keys: " << ht->numKeys << endl;
	}
	ht->addKeyValue(key,value);
	cout << endl;
	infile.close();
}

void getTweet::writeFile() {
	cout << "HEY IM WRITING HERE" << endl;
	ofstream outfile(newfile.c_str(),ios::out);

	outfile << ht->first << " ";
	string key = "";
	string value = ht->map[ht->getIndex(ht->first)]->getRandValue();
	int ct = ht->first.length() + 1 + 5; //character count init - first word added plus a space plus a buffer
	int len = 0;
	while (ct < 279) { // max length for a tweet
		key = value;
		outfile << key << " ";
		if (len == 11) {
			outfile << "\n";
			len = 0;
			ct++; // new line = new character
		}
		else len++;
		ct = ct + key.length() + 1; // add the char length of the word plus a space
		value = ht->map[ht->getIndex(key)]->getRandValue();
	}
	cout << "IM DONE WRITING HERE" << endl;
	outfile.close();
}

bool getTweet::fileCheck (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

int main() {
	srand(time(NULL));

	int debug = 0;

	// Testing to make sure hashNodes work
	if (debug){
		hashNode n = hashNode("Punctuation", "Comma");
		n.addValue("Period");
		n.addValue("Semicolon");
		n.addValue("Quotation");
		n.addValue("Colon");
		cout << n.getRandValue() << endl;
	}

	getTweet newTweet = getTweet(true, true);
}
