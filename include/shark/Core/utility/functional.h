/*!
 *  \brief Small General algorithm collection.
 *
 *
 *  \author  Oswin Krause
 *  \date    2012
 *
 *  <BR><HR>
 *  This file is part of Shark. This library is free software;
 *  you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software
 *  Foundation; either version 3, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SHARK_CORE_FUNCTIONAL_H
#define SHARK_CORE_FUNCTIONAL_H

#include <boost/range/numeric.hpp>
#include <boost/range/algorithm/nth_element.hpp>
#include <boost/bind.hpp>
#include <shark/Core/utility/Iterators.h>
#include <algorithm>
#include <shark/Rng/GlobalRng.h>
namespace shark{
	
///\brief random_shuffle algorithm which stops after acquiring the random subsequence for [begin,middle)
template<class RandomAccessIterator, class Rng>
void partial_shuffle(RandomAccessIterator begin, RandomAccessIterator middle, RandomAccessIterator end, Rng& rng){
	std::random_shuffle(begin,end,rng);
	// todo: test the algorithm below!
	//~ typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
	//~ difference_type n = middle - begin;
	//~ for (; begin != middle; ++begin,--n) {
		
		//~ using std::swap;
		//~ swap(*begin, begin[rng(n)]);
	//~ }
}

///\brief random_shuffle algorithm which stops after acquiring the random subsequence for [begin,middle)
template<class RandomAccessIterator>
void partial_shuffle(RandomAccessIterator begin, RandomAccessIterator middle, RandomAccessIterator end){
	DiscreteUniform<Rng::rng_type> uni(Rng::globalRng,0,1);
	partial_shuffle(begin,middle,end,uni);
}

///\brief Given two Ranges of labels and predictions and an ErrorMeasure, calculates the accumulated error.
template<class Range1, class Range2, class ErrorMeasure>
double accumulateError(Range1 const& labels, Range2 const& predictions, ErrorMeasure error){
	return boost::inner_product(labels,predictions,0.0,std::plus<double>(),error);
}


///\brief Returns the iterator to the median element. after this call, the range is partially ordered.
///
///After the call, all elements left of the median element are
///guaranteed to be <= median and all element on the right are >= median.

template<class Range>
typename boost::range_iterator<Range>::type median_element(Range& range){
	typedef typename boost::range_iterator<Range>::type iterator;

	std::size_t size = shark::size(range);
	std::size_t medianPos = (size+1)/2;
	iterator medianIter = boost::begin(range)+medianPos;

	boost::nth_element(range,medianIter);

	return medianIter;
}

template<class Range>
typename boost::range_iterator<Range>::type median_element(Range const& rangeAdaptor){
	Range adaptorCopy(rangeAdaptor);
	return median_element(adaptorCopy);
}
///\brief Partitions a range in two parts as equal in size as possible.
///
///The Algorithm partitions the range and returns the splitpoint. The elements in the range
///are ordered such, that at the end all elements in [begin,splitpoint)< [splitpoint,end]
///this partition is done such, that the ranges are as equally sized as possible.
///It is guaranteed, that the left range is not empty. However, if the range consists only
///of equal elements, the return value will be the end iterator indicating that there is no
///split possible.
///The whole algorithm runs in linear time by iterating 2 times over the sequence.
template<class Range>
typename boost::range_iterator<Range>::type partitionEqually(Range& range){
	typedef typename boost::range_iterator<Range>::type iterator;
	typedef typename boost::iterator_value<iterator>::type value_type;

	iterator begin = boost::begin(range);
	iterator end = boost::end(range);
	iterator medianIter = median_element(range);

	//in 99% of the cases we would be done right now. in the remaining 1% the median element is
	//not unique so we partition the left and the right such that all copies are ordered in the middle
	iterator left = std::partition(begin,medianIter,boost::bind(std::less<value_type>(),_1,*medianIter));
	iterator right = std::partition(medianIter,end,boost::bind(std::equal_to<value_type>(),_1,*medianIter));

	//we guarantee, that the left range is not empty
	if(left == begin){
		return right;
	}
	//now we return left or right depending on which is nearer to the median
	if(medianIter-left <= right - medianIter)
		return left;
	else
		return right;
}

///\brief Partitions a range in two parts as equal in size as possible and returns it's result
///
///This the verison for adapted ranges.
template<class Range>
typename boost::range_iterator<Range>::type partitionEqually(Range const& rangeAdaptor){
	Range adaptorCopy(rangeAdaptor);
	return partitionEqually(adaptorCopy);
}
}
#endif