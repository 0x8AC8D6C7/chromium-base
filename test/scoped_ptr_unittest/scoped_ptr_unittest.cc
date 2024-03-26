#include "base/memory/scoped_ptr.h"
#include "base/basictypes.h"
#include "base/bind.h"
#include "base/callback.h"

#include <gtest/gtest.h>

TEST(InitGtest, TestName) {
    EXPECT_EQ(1, 1);
}
namespace {

// Used to test depth subtyping.
class ConDecLoggerParent {
 public:
  virtual ~ConDecLoggerParent() {}

  virtual void SetPtr(int* ptr) = 0;

  virtual int SomeMeth(int x) const = 0;
};

class ConDecLogger : public ConDecLoggerParent {
 public:
  ConDecLogger() : ptr_(nullptr) { }
  explicit ConDecLogger(int* ptr) { SetPtr(ptr); }
  virtual ~ConDecLogger() { --*ptr_; }

  virtual void SetPtr(int* ptr) OVERRIDE { ptr_ = ptr; ++*ptr_; }

  // just return the given number
  virtual int SomeMeth(int x) const OVERRIDE { return x; }

 private:
  int* ptr_;

  DISALLOW_COPY_AND_ASSIGN(ConDecLogger);
};

}  // namespace

TEST(ScopedPtrTest, ScopedPtr) {
  int constructed = 0;

  // Ensure size of scoped_ptr<> doesn't increase unexpectedly.
  COMPILE_ASSERT(sizeof(int*) >= sizeof(scoped_ptr<int>),
                 scoped_ptr_larger_than_raw_ptr);

  {
    scoped_ptr<ConDecLogger> scoper(new ConDecLogger(&constructed));
    EXPECT_EQ(1, constructed);
    EXPECT_TRUE(scoper.get());

    EXPECT_EQ(10, scoper->SomeMeth(10));
    EXPECT_EQ(10, scoper.get()->SomeMeth(10));
    EXPECT_EQ(10, (*scoper).SomeMeth(10));
  }
  EXPECT_EQ(0, constructed);

  // Test reset() and release()
  {
    scoped_ptr<ConDecLogger> scoper(new ConDecLogger(&constructed));
    EXPECT_EQ(1, constructed);
    EXPECT_TRUE(scoper.get());

    scoper.reset(new ConDecLogger(&constructed));
    EXPECT_EQ(1, constructed);
    EXPECT_TRUE(scoper.get());

    scoper.reset();
    EXPECT_EQ(0, constructed);
    EXPECT_FALSE(scoper.get());

    scoper.reset(new ConDecLogger(&constructed));
    EXPECT_EQ(1, constructed);
    EXPECT_TRUE(scoper.get());

    ConDecLogger* take = scoper.release();
    EXPECT_EQ(1, constructed);
    EXPECT_FALSE(scoper.get());
    delete take;
    EXPECT_EQ(0, constructed);

    scoper.reset(new ConDecLogger(&constructed));
    EXPECT_EQ(1, constructed);
    EXPECT_TRUE(scoper.get());
  }
  EXPECT_EQ(0, constructed);

  // Test swap(), == and !=
  {
    scoped_ptr<ConDecLogger> scoper1;
    scoped_ptr<ConDecLogger> scoper2;
    EXPECT_TRUE(scoper1 == scoper2.get());
    EXPECT_FALSE(scoper1 != scoper2.get());

    ConDecLogger* logger = new ConDecLogger(&constructed);
    scoper1.reset(logger);
    EXPECT_EQ(logger, scoper1.get());
    EXPECT_FALSE(scoper2.get());
    EXPECT_FALSE(scoper1 == scoper2.get());
    EXPECT_TRUE(scoper1 != scoper2.get());

    scoper2.swap(scoper1);
    EXPECT_EQ(logger, scoper2.get());
    EXPECT_FALSE(scoper1.get());
    EXPECT_FALSE(scoper1 == scoper2.get());
    EXPECT_TRUE(scoper1 != scoper2.get());
  }
  EXPECT_EQ(0, constructed);
}