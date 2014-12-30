#ifndef CPPJIEBA_TRIE_HPP
#define CPPJIEBA_TRIE_HPP

#include "Limonp/StdExtension.hpp"
#include <vector>
#include "Rcpp.h"

using namespace Rcpp;

namespace CppJieba
{
using namespace std;
template <class KeyType, class ValueType>
class TrieNode
{
public:
    typedef unordered_map<KeyType, TrieNode<KeyType, ValueType>* > KeyMapType;
public:
    KeyMapType *ptKeyMap;
    const ValueType *ptValue;
};

template <class KeyType, class ValueType, class KeyContainerType = vector<KeyType>, class KeysContainerType = vector<KeyContainerType>, class ValueContainerType = vector<const ValueType * > >
class Trie
{
public:
    typedef TrieNode<KeyType, ValueType> TrieNodeType;
private:
    TrieNodeType *_root;
public:
    Trie(const KeysContainerType &keys, const ValueContainerType &valuePointers)
    {
        _root = new TrieNodeType;
        _root->ptKeyMap = NULL;
        _root->ptValue = NULL;

        _createTrie(keys, valuePointers);
    }
    ~Trie()
    {
        if (_root)
        {
            _deleteNode(_root);
        }
    }
public:
    const ValueType *find(typename KeyContainerType::const_iterator begin, typename KeyContainerType::const_iterator end) const
    {
        typename TrieNodeType::KeyMapType::const_iterator citer;
        const TrieNodeType *ptNode = _root;
        for (typename KeyContainerType::const_iterator it = begin; it != end; it++)
        {
            if (!(ptNode))
            {
                stop("!(ptNode)  Trie.hpp : 52");
            }

            if (NULL == ptNode->ptKeyMap || ptNode->ptKeyMap->end() == (citer = ptNode->ptKeyMap->find(*it)))
            {
                return NULL;
            }
            ptNode = citer->second;
        }
        return ptNode->ptValue;
    }
  bool find(
      typename KeyContainerType::const_iterator begin, 
      typename KeyContainerType::const_iterator end, 
      std::vector<std::pair<typename KeyContainerType::size_type, const ValueType* > >& res, 
      size_t offset = 0) const
  {
        const TrieNodeType *ptNode = _root;
        typename TrieNodeType::KeyMapType::const_iterator citer;
        for (typename KeyContainerType::const_iterator itr = begin; itr != end ; itr++)
        {
            if (!(ptNode))
            {
                stop("!(ptNode)  Trie.hpp : 52");
            }
            if (NULL == ptNode->ptKeyMap || ptNode->ptKeyMap->end() == (citer = ptNode->ptKeyMap->find(*itr)))
            {
                break;
            }
            ptNode = citer->second;
            if (ptNode->ptValue)
            {
              if(itr == begin && res.size() == 1) // first singleword
              {
                res[0].second = ptNode->ptValue;
              }
              else
              {
                res.push_back(pair<typename KeysContainerType::size_type, const ValueType* >(itr - begin + offset, ptNode->ptValue));
              }
            }
        }
        return !res.empty();
    }
private:
    void _createTrie(const KeysContainerType &keys, const ValueContainerType &valuePointers)
    {
        if (valuePointers.empty() || keys.empty())
        {
            return;
        }
        if (!(keys.size() == valuePointers.size()))
        {
            stop("keys.size() != valuePointers.size() Trie.hpp : 52");
        }

        for (size_t i = 0; i < keys.size(); i++)
        {
            _insertNode(keys[i], valuePointers[i]);
        }
    }
private:
    void _insertNode(const KeyContainerType &key, const ValueType *ptValue)
    {
        TrieNodeType *ptNode  = _root;

        typename TrieNodeType::KeyMapType::const_iterator kmIter;

        for (typename KeyContainerType::const_iterator citer = key.begin(); citer != key.end(); citer++)
        {
            if (NULL == ptNode->ptKeyMap)
            {
                ptNode->ptKeyMap = new typename TrieNodeType::KeyMapType;
            }
            kmIter = ptNode->ptKeyMap->find(*citer);
            if (ptNode->ptKeyMap->end() == kmIter)
            {
                TrieNodeType *nextNode = new TrieNodeType;
                nextNode->ptKeyMap = NULL;
                nextNode->ptValue = NULL;

                (*ptNode->ptKeyMap)[*citer] = nextNode;
                ptNode = nextNode;
            }
            else
            {
                ptNode = kmIter->second;
            }
        }
        ptNode->ptValue = ptValue;
    }
    void _deleteNode(TrieNodeType *node)
    {
        if (!node)
        {
            return;
        }
        if (node->ptKeyMap)
        {
            typename TrieNodeType::KeyMapType::iterator it;
            for (it = node->ptKeyMap->begin(); it != node->ptKeyMap->end(); it++)
            {
                _deleteNode(it->second);
            }
            delete node->ptKeyMap;
        }
        delete node;
    }
};
}

#endif