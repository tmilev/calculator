#include "general_logging_global_variables.h"
#include "test.h"
#include <iostream>

bool HashedListTest::all() {
  global << "Testing hashed lists..." << Logger::endL;
  HashedListTest::largeHashedListReferences1(1);
  HashedListTest::largeHashedListReferences1(2);
  HashedListTest::largeHashedListReferences1(10);
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
  int iterations = 1 * 1000 * 1000;
  int current = 0;
  int64_t millisecondsStart = global.getElapsedMilliseconds();
  int hashResizesStart = GlobalStatistics::numberOfHashResizes;
  int listResizesStart = GlobalStatistics::numberOfListResizesTotal;
  for (int i = 0; i < iterations; i ++) {
    large.addOnTop(current);
    current = HashedListTest::incrementIntPseudorandomly(current);
  }
  int64_t duration = global.getElapsedMilliseconds() - millisecondsStart;
  int64_t maximumDuration = iterations / 500;
  if (duration > maximumDuration) {
    global.fatal
    << "Hashed list of size "
    << iterations
    << " took "
    << duration
    << " milliseconds to construct, maximum: "
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
    << ". Ram used by hash buckets: "
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

bool HashedListTest::largeHashedListReferences1(int millionMultiple) {
  STACK_TRACE("HashedListTest::largeHashedList1");
  HashedListReferences<int> large;
  int iterations = millionMultiple * 1000 * 1000;
  int64_t millisecondsStart = global.getElapsedMilliseconds();
  int hashResizesStart = GlobalStatistics::numberOfHashResizes;
  int listResizesStart = GlobalStatistics::numberOfListResizesTotal;
  for (int i = 0; i < iterations; i ++) {
    large.addOnTop(i);
  }
  int64_t duration = global.getElapsedMilliseconds() - millisecondsStart;
  int64_t maximumDuration = millionMultiple * 2000;
  int64_t recommendedDuration = millionMultiple * 100;
  if (duration > maximumDuration) {
    global.fatal
    << "Hashed list of size "
    << iterations
    << " took "
    << duration
    << " milliseconds to construct, maximum: "
    << maximumDuration
    << ". "
    << global.fatal;
  }
  if (duration > recommendedDuration) {
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
    << large.getReport()
    << "Ram consumed: "
    << RamUsageComputation::byteSize(large)
    << ". Ram used by hash buckets: "
    << RamUsageComputation::byteSize(large.hashBuckets)
    << Logger::endL;
  }
  return true;
}

bool ListReferencesTest::all() {
  STACK_TRACE("ListReferencesTest::all");
  global << "List references test start. " << Logger::endL;
  ListReferencesTest::largeListReferences1(1);
  ListReferencesTest::largeListReferences1(2);
  ListReferencesTest::largeListReferences1(4);
  ListReferencesTest::quicksortReversedList(100);
  ListReferencesTest::quicksortReversedList(10000);
  global
  << Logger::green
  << "List references test completed. "
  << Logger::endL;
  return true;
}

bool ListReferencesTest::quicksortReversedList(int numberOfElements) {
  ListReferences<int> elements;
  for (int i = 0; i < numberOfElements; i ++) {
    elements.addOnTop(numberOfElements - i - 1);
  }
  elements.quickSortAscending();
  return true;
}

bool ListReferencesTest::largeListReferences1(int millionMultiple) {
  STACK_TRACE("ListReferencesTest::largeListReferences1");
  ListReferences<int> list;
  int iterations = millionMultiple * 1000 * 1000;
  int64_t startMilliseconds = global.getElapsedMilliseconds();
  for (int i = 0; i < iterations; i ++) {
    list.addOnTop(i);
  }
  int64_t duration = global.getElapsedMilliseconds() - startMilliseconds;
  int64_t maximumDuration = millionMultiple * 500;
  int64_t recommendedDuration = millionMultiple * 50;
  if (duration > maximumDuration) {
    global.fatal
    << "List references with "
    << iterations
    << " elements took "
    << duration
    << " milliseconds to construct, expected maximum of "
    << maximumDuration
    << ". "
    << global.fatal;
  }
  if (duration > recommendedDuration) {
    global
    << Logger::red
    << "List references with "
    << iterations
    << " elements took "
    << duration
    << " milliseconds to construct, recommended maximum of "
    << recommendedDuration
    << ". "
    << list.getMemoryReport()
    << Logger::endL;
  }
  return true;
}
