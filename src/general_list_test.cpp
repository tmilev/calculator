#include "general_logging_global_variables.h"
#include "test.h"
#include <iostream>

bool HashedListTest::all() {
  HashedListTest::largeHashedList1();
  HashedListTest::largeHashedList2();
  return true;
}

int HashedListTest::incrementIntPseudorandomly(int counter) {
  int64_t result = static_cast<int64_t>(counter);
  if (result == 0) {
    result = 1;
  }
  result *= 19;
  result %= 123456791;
  return static_cast<int>(result);
}

bool HashedListTest::largeHashedList1() {
  STACK_TRACE("HashedListTest::largeHashedList1");
  HashedList<int> large;
  int iterations = 1000000;
  int current = 0;
  int64_t millisecondsStart = global.getElapsedMilliseconds();
  int hashResizesStart = GlobalStatistics::numberOfHashResizes;
  int listResizesStart = GlobalStatistics::numberOfListResizesTotal;
  for (int i = 0; i < iterations; i ++) {
    large.addOnTop(current);
    current = HashedListTest::incrementIntPseudorandomly(current);
  }
  int64_t duration = global.getElapsedMilliseconds() - millisecondsStart;
  int64_t maximumDuration = 2000;
  if (duration > maximumDuration) {
    global.fatal
    << "Hashed list of size "
    << iterations
    << " took "
    << duration
    << " milliseconds to consotruct, maximum: "
    << maximumDuration
    << ". "
    << global.fatal;
  }
  if (duration > 100) {
    global
    << Logger::yellow
    << "Hashed list with "
    << iterations
    << " pseudorandoms took "
    << duration
    << " milliseconds to construct. "
    << Logger::endL
    << large.getReport()
    << Logger::endL
    << "Total list resizes: "
    << GlobalStatistics::numberOfListResizesTotal - listResizesStart
    << ". "
    << "Total hash resizes: "
    << GlobalStatistics::numberOfHashResizes - hashResizesStart
    << Logger::endL
    << "Ram consumed: "
    << RamUsageComputation::byteSize(large)
    << "Ram used by hash buckets: "
    << RamUsageComputation::byteSize(large.hashBuckets)
    << Logger::endL;
  }
  return true;
}

bool HashedListTest::largeHashedList2() {
  STACK_TRACE("HashedListTest::largeHashedList2");
  HashedList<HashedList<int> > large;
  HashedList<int> differentIntegers;
  int iterations = 1000000;
  int current = 0;
  int64_t millisecondsStart = global.getElapsedMilliseconds();
  for (int i = 0; i < iterations; i ++) {
    HashedList<int> incoming;
    incoming.addOnTop(current);
    current = HashedListTest::incrementIntPseudorandomly(current);
    incoming.addOnTop(current);
    large.addOnTop(incoming);
    differentIntegers.addOnTopNoRepetition(current);
  }
  int64_t duration = global.getElapsedMilliseconds() - millisecondsStart;
  int64_t maximumDuration = 10000;
  int64_t byteSize = RamUsageComputation::byteSize(large);
  int64_t maximumByteSize = iterations * 240;
  if (duration > maximumDuration) {
    global.fatal
    << "Hashed of hashed lists of size "
    << iterations
    << " took "
    << duration
    << " milliseconds to construct, maximum: "
    << maximumDuration
    << ". "
    << global.fatal;
  }
  if (byteSize > maximumByteSize) {
    global.fatal
    << "RAM memory consumption: "
    << byteSize
    << " is larger than the maximum allowed: "
    << maximumByteSize
    << "\nHash report one item: "
    << large.lastObject()->getReport()
    << "\nHash report, full list: "
    << large.getReport()
    << "\nDifferent integers: "
    << differentIntegers.size
    << global.fatal;
  }
  if (duration > 100) {
    global
    << Logger::yellow
    << "Large hash list of hashed lists with "
    << iterations
    << " elements took "
    << duration
    << " milliseconds to construct. "
    << Logger::endL
    << large.getReport()
    << Logger::endL
    << "Ram consumed: "
    << RamUsageComputation::byteSize(large)
    << Logger::endL;
  }
  return true;
}
