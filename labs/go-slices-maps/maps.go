package main

import (
	"strings"

	"golang.org/x/tour/wc"
)

// WordCount returns a map with the times a word appears in the string.
func WordCount(s string) map[string]int {
	words := strings.Fields(s)
	wordMap := make(map[string]int)
	for w := range words {
		word := words[w]
		elem, present := wordMap[word]
		if present {
			wordMap[word] = elem + 1
		} else {
			wordMap[word] = 1
		}
	}
	return wordMap
}

func main() {
	wc.Test(WordCount)
}
