/// counts number of primes from standard input
///
/// compile with:
///   $ gcc countPrimes.c -O2 -o count -lm -lpthread

///
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>

#define DEBUG 0
#define DEBUG2 0


int nThreads = 1;
pthread_mutex_t lock;
pthread_t *threads;
int counter = 0;
bool foundNonPrime = false;

/// primality test, if toCheck is prime, return 1, else return 0
int isPrime(int64_t toCheck);

void *primeChecker(void *arg);

struct ThreadArguments {
	int64_t startNumber;
	int64_t endNumber;
	int64_t isPrimeNumber;
	int64_t myThreadNumber;
};


int main(int argc, char **argv) {
/*    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        exit(-1);
    }*/

	/// parse command line arguments
	if (argc != 1 && argc != 2) {
		printf("Uasge: countPrimes [nThreads]\n");
		exit(-1);
	}
	if (argc == 2) nThreads = atoi(argv[1]);

	/// handle invalid arguments
	if (nThreads < 1 || nThreads > 256) {
		printf("Bad arguments. 1 <= nThreads <= 256!\n");
	}
/*    if( nThreads != 1) {
        printf("I am not multithreaded yet :-(\n");
        exit(-1);
    }*/

	threads = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);

	/// count the primes
	printf("Counting primes using %d thread%s.\n",
	       nThreads, nThreads == 1 ? "s" : "");
	int64_t count = 0;
	while (1) {
		int64_t num;
		if (1 != scanf("%ld", &num)) {
			break;
		}

#if DEBUG
		printf("Checking number %ld\n", num);
#endif

		if (isPrime(num)){
#if DEBUG2
			printf("%ld is prime!\n", num);
#endif
			count++;
		} else {
#if DEBUG2
			printf("%ld is composite!\n", num);
#endif
		}
	}

	/// report results
	printf("Found %ld primes.\n", count);

	return 0;
}

int isPrime(int64_t toCheck) {
	int pid[nThreads];
	struct ThreadArguments myThreadArgs[nThreads];


	if (toCheck <= 1) return 0; // small numbers are not primes
	if (toCheck <= 3) return 1; // 2 and 3 are prime
	if (toCheck % 2 == 0 || toCheck % 3 == 0) return 0; // multiples of 2 and 3 are not primes
	int64_t currentIndex = 5;
	int64_t max = sqrt(toCheck);

#if DEBUG
	printf("Max is %ld\n", max);
#endif+


	if (max < 100){
		while( currentIndex <= max) {
			if (toCheck % currentIndex == 0 || toCheck % (currentIndex+2) == 0) return 0;
			currentIndex += 6;
		}
		return 1;
	}




	int64_t increment = round(max / 6 / nThreads) * 6;

	//deal with simple primes


	for (int k = 0; k < nThreads && currentIndex < max; ++k) {
		myThreadArgs[k].myThreadNumber = k;
		myThreadArgs[k].isPrimeNumber = toCheck;
		myThreadArgs[k].startNumber = currentIndex;
		if (k == nThreads - 1) {
			myThreadArgs[k].endNumber = max;
		} else {
			//remainingGroupsSize
			myThreadArgs[k].endNumber = currentIndex + increment;
		}
		currentIndex += increment;
		pid[k] = pthread_create(&threads[k], NULL, primeChecker, (void *) &myThreadArgs[k]);
	}

	for (int j = 0; j < nThreads; ++j) {
		int64_t ret = 0;
		pthread_join(threads[j], (void **) ret);
		counter = counter + (int) ret;
	}


	if (foundNonPrime) {
		foundNonPrime = false;
		return 0;
	} else {
		return 1;
	}
}


void *primeChecker(void *arg) {
	struct ThreadArguments myArguments = *(struct ThreadArguments *) arg;
	int64_t start = myArguments.startNumber;
	int64_t end = myArguments.endNumber;
	const int64_t isPrime = myArguments.isPrimeNumber;

#if DEBUG
	printf("Thread %ld, Start: %ld, End: %ld, on %ld\n", myArguments.myThreadNumber, start, end, isPrime);
#endif

	/*    while( i <= max) {
        if (n % i == 0 || n % (i+2) == 0) {
            return 0;
        }
        i += 6;
    }*/

	while (!foundNonPrime && start <= end) {
		if ((isPrime % start == 0 || isPrime % (start + 2) == 0)) {
#if DEBUG
			printf("Found prime factorization at %ld!\n", start);
#endif
			foundNonPrime = true;
			return 0;
		}
		start += 6;
	}

	pthread_exit((void *) 1);
}


