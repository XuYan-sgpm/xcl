//
// Created by 徐琰 on 2023/9/1.
//
#include <gtest/gtest.h>
#include <xcl/util/list.h>

class ListTest : public ::testing::Test {
 protected:
  struct IntNode : public ListNode {
    int value;

    explicit IntNode(int v) : value(v), ListNode({nullptr, nullptr}) {}
  };

  List list_;
  ListNode header_;

  void SetUp() override {
    Test::SetUp();
    List_init(&list_);
    memset(&header_, 0, sizeof(header_));
  }
  void TearDown() override {
    Test::TearDown();
    auto node = List_first(&list_);
    while (node != List_last(&list_)) {
      auto next = node->next;
      delete (IntNode*)node;
      node = next;
    }
    node = header_.next;
    while (node) {
      ListNode* next = node->next;
      delete (IntNode*)node;
      node = next;
    }
  }

  static ListNode* newNode(int val) {
    auto* node = new IntNode(val);
    return node;
  }

  int getRand() {
    srand(rand() + time(nullptr));
    return rand();
  }

  static int _compare(const ListNode* node1, const ListNode* node2) {
    return ((IntNode*)node1)->value - ((IntNode*)node2)->value;
  }

  static void printList(List* list) {
    ListNode* node = List_first(list);
    std::cout << ((IntNode*)node)->value;
    while ((node = node->next) != List_last(list)) {
      std::cout << ' ' << ((IntNode*)node)->value;
    }
    std::cout << std::endl;
  }

  static void printList2(ListNode* node) {
    std::cout << ((IntNode*)node)->value;
    while ((node = node->next)) {
      std::cout << ' ' << ((IntNode*)node)->value;
    }
    std::cout << std::endl;
  }
};

TEST_F(ListTest, func1) {
  int n = 1000;
  for (int i = 0; i < n; i++) {
    auto node = newNode(getRand());
    List_addBack(&list_, node);
  }
  //        printList(&list_);
  List_sort2(&list_, _compare);
  int prev = 1 << 31;
  ListNode* node = List_first(&list_);
  ASSERT_EQ(list_.size, n);
  for (int i = 0; i < n; i++) {
    ASSERT_LE(prev, ((IntNode*)node)->value);
  }
  //    std::cout << "after sort" << std::endl;
  //    printList(&list_);
}

TEST_F(ListTest, func2) {
  int n = 1000;
  ListNode* cur = &header_;
  for (int i = 0; i < n; i++) {
    auto node = newNode(getRand());
    List_link(cur, node);
    cur = node;
  }
  //    printList2(header_.next);
  header_.next = List_sort(header_.next, _compare);
  int prev = 1 << 31;
  cur = header_.next;
  for (int i = 0; i < n; i++) {
    ASSERT_LE(prev, ((IntNode*)cur)->value);
    cur = cur->next;
  }
  ASSERT_EQ(cur, nullptr);
}
