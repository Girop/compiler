# Dependencies
- NASM

# Compiler todos
- [] Improve ast dumping
- [] Try with using Upsilon in place of parametrized Phis 
- [] Fix sealed & filled flags
- [] SSA construction alogorithm needs major revamp, blocks are sealed too late and we end up with producing too many
phis, which are immediately removed by 'removeTrivialPhis'. 'Blocks' class should provide an easier interafce, which will allow to
do smarter filling with the SSA instructions. Plus the "Marker algorithm" (see the paper) can be used if this still will be a problem.
And there is also the SCC removal algo.
First look and measure how big of a problem this will be for real, bigger C programmes.
TODO look at the variable reading memoization technique.
