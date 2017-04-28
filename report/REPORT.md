Background
==========

There are a multitude of electoral systems in use around the world
today, and many more variants and theoretical systems not in wide use.
While simple plurality or majority elections such as those that are
common in America are fairly simple to count, more sophisticated systems
can become quite complex to count accurately, especially those that
allow ranked-choice voting, where voters number candidates in order of
their preference, which can require many iterative rounds of recounting
and reallocation. And though there is an increasing role for electronic
counting, hand counts remain the gold standard, especially for recounts.
While vote-counting algorithms are typically understood as serial
algorithms, large elections are also, in some sense, distributed, as
typically votes are collected and counted in local precincts and results
reported to a higher election authority. Our project was an attempt to
explore the possibilities and challenges that face electronic
vote-counting systems by implementing a distributed vote-counter.

Voting Systems
--------------

There are a variety of popular voting systems and variants, but for our
project, we decided to focus on four. The first is plurality
first-past-the-post (FPTP), a simple, single-winner system in which the
foremost vote-getter wins. The second voting system is party-list
proportional representation (list PR), which divides a number of winning
positions (e.g. congressional or parliamentary seats) proportionally
between candidates or parties based on the vote their party’s list
received. This system can also be used to allocate seats based on
population, as with the US House of Representatives. There are a number
of variants of list PR that calculate the quota of votes required to win
a seat differently, which has subtle but potentially major effects on
the outcome of the election. We chose to implement the D’Hondt highest
average method, a common quota calculation method that awards seats
according to the following formula:
quota=n_votes / (n_seats + 1)

The third voting system that we implemented is single-winner
ranked-choice voting, also known as instant-runoff voting (IRV) or
preferential voting. In IRV, voters rank their choices and low-scoring
candidates are successively eliminated and their votes reallocated to
lower preferences until a candidate has an absolute majority of votes.
The forth and last system we implemented is single-winner ranked-choice
voting, also know as single transferable vote (STV), which is by far the
most complex system we implemented. STV, like list PR, results in
proportional representation, but through a process more similar to IRV.
There are many variants and different ways of counting STV, most of
which can change the outcome of an election, so we decided to follow the
recommendations of the Wright System, an iterative algorithm for
counting STV. We regard STV as the core voting system of our project,
and was originally conceived as the sole focus of the project. One
interesting aspect of STV is that when a candidate is declared a winner,
all of their votes in excess of the winning quota are reallocated to
their next preference, but only as fractions proportional to how far
beyond the quota their previous winning candidates were. This means that
votes can quickly become worth only hundredths of a full vote. However,
at large scales, millions of votes, even fractional ones, still matter
quite a lot, so we decided fractional precision was paramount for STV.
This decision had a major impact on how we wrote our code, as detailed
later in the Methods section.

Goals
-----

There are two main properties we consider to be essential to our vote
counter. The first is accuracy and reproducibility. There is absolutely
no room for error when in comes to elections in real life, nor should
there be in any remotely serious simulation. This meant taking great
pains to catch regressions and create a robust, stable program, and
avoiding potential sources of error like floating-point numbers.

The other goal is efficient communication. Transferring data between
nodes is expensive, so every piece of data we send should be essential.
If some data may not be needed, we should defer sending until it is
definitely needed. Our performance depends in large part on how much we
can reduce communication to the bare minimum.

![image](votecharts1.png)
![image](votecharts2.png)

**Figures 1 and 2:** Sample HTML output for each system using the same
input.

Methods
=======

For this project we worked mainly in C, with some shell scripts and a
Python vote generator for convenience, and used MPI (specifically,
MPICH) to distribute our program. Also of note is the GMP library, which
we used when possible for rational numbers.

Our first task was to implement a data input format, a command-line
interface, and, of course, serial versions of all of our voting systems.
This was a very incremental process. Though voting systems are often
described in algorithmic terms, we found it more difficult than expected
to convert from these high-level algorithms used for hand counts to
safe, efficient C code. Though we expected implementing STV in
particular to be a large task, we found even the simpler systems to be
rife with potential pitfalls and edge cases. The vote parser and data
format also turned out to be more work than expected. For these reasons,
the serial phase of our project took far longer than we wanted to become
stable enough to build on. As we worked on the voting algorithms, we
also implemented HTML table output to examine results in detail and an
ad-hoc testing framework to catch regressions. It would be hard to
exaggerate how helpful these systems were, even as basic sanity tests,
especially when making changes that affected all voting systems.

Once the serial algorithms were stable and reliable, we were able to
incrementally distribute each voting system. Unsurprisingly, FPTP and
list PR, which have only one round of counting, were very simple. On the
other hand, IRV and STV were more complicated. While we eventually
landed on an efficient communication pattern for IRV, STV in particular
was tricky; our serial STV implementation often required quite a lot of
information about the global state of the count in order to progress, so
we were often forced to compromise between additional communication and
redundant computation. One problem that, in retrospect, was inevitable
was the interaction of GMP’s allocation-heavy, arbitrary-precision
rationals and MPI communication, which is all about sending fixed
amounts of data. Though we tried to figure out a way to serialize
rationals and implement a custom reduce function, we eventually decided
to accept some loss of precision and temporarily store our rationals as
long doubles. These were precise enough to pass all of our tests, and it
is likely that some other part of our code would break before we got to
an input large enough to lose precision in any notable way, but it was a
compromise that we wanted to avoid.

We originally intended to create an online vote-counter that would
update results as it read in new data, but we simply did not have the
time after our serial phase went long. We started on design work for the
online system and intended to build a threaded client-server model to
handle new work arriving, but we were not able to get it in a working
state in time. Perhaps with another week or two we could finish that
aspect of the project, but unfortunately we just barely managed to get
the basic MPI distribution in a working state days before the project
deadline.

Experiments and Results
=======================

To measure our performance gains from distribution, we simply measured
serial and distributed times with different datasets. We implemented the
distribution in such a way that running the MPI version of the program
on one node ran practically identically to the serial version, so we
were able to get all of our results from a single version of the
program, which was convenient.

Before we discuss the results, however, there are some caveats. We were
exclusively interested in the performance of the vote-counting
algorithms, but in the end those algorithms only accounted for about a
third to a sixth of the total program runtime, depending on the input;
the rest of the time was largely spent initializing and filling the data
structures used to hold the votes. We measured the counting algorithms
separately from total runtime in order to make our charts and draw
conclusions, but we feel it is worth mentioning that our approach to
initializing data was fairly slow, even though it was not really a core
part of the project.

The other caveat is that the inputs our charts are based on all have
uniformly-distributed data, and so represent the best-case scenario. In
the worst case, with almost all of the data on one node, performance
would be equal or worse than the serial version with the same inputs.
The data we used to time our program is available in our source
repository along with our code. It is likely that changing some details
of the inputs, like the number of candidates or number of winners (if
applicable) might cause the results to change somewhat, but the total
number of votes is far and away the most significant variable.

![image](strong.png)
![image](weak.png)
**Figures 3 and 4:** Speedup of our various counting algorithms. List
and FPTP may be hard to see, as they almost exactly overlap in both
charts

In terms of scaling, our results were decent. We were mainly concerned
with weak scaling, but we obtained some fairly good strong scaling as
well, with all of our algorithms scaling very well up to and beyond
eight nodes. Since counting can very easily be divided most of the time,
this is not surprising. Interestingly, STV takes about six times as long
in total as IRV for the same inputs, which is not surprising, but they
have very similar rates of speedup, and STV even seems to parallelize
better, which is a little bit surprising. FPTP and list PR had almost
exactly identical runtimes and speedup, so much so that it is hard to
distinguish them on the chart.

Weak scaling is good, but slightly odd. Performance degrades
substantially up to eight nodes, then levels out almost completely.
Assuming this pattern remains consistent for even greater numbers of
nodes, consistency is definitely very good, especially for STV, which
settled at just under 0.7 speedup, which is not bad at all. FPTP and
list PR stay at just under 0.5 speedup, but they run so fast in the
first place that they do not really need weak scaling that much anyway.
We feel that these are good results overall, especially considering that
there is still room for improvement and optimizations in the serial
code.

Conclusions and Future Work
===========================

In general, we are satisfied with how our project turned out. Though the
fact that it remains incomplete according to the original plan is
disappointing, that original plan was probably far too optimistic to
begin with. If we had made more detailed and thoughtful plans from the
beginning, we probably would have accomplished more, but neither of us
really knew exactly what to expect from the project. One thing that
really held us back was the difficulty of splitting up work. Since the
early phases of the serial program, what you might call the minimum
valid program that can take input and give output, involved several
tightly coupled systems, we could not effectively work on them
separately without clobbering each other’s work. Even later on, we
changed headers and data structures so often that even commits that
should have been fairly atomic ended up touching everything. For an
example of this, see our HTML output system, which relies on coordinated
file output in our main file and our vote system files; while effective
and simple, it ended up as a tightly-coupled burden. We ended up working
together by pair programming and splitting up writing duties for
non-code deliverables, which was useful, but if we had planned out a
more modular architecture or established semi-stable headers for each
file and split up coding work, we probably could have accomplished more.
While this was an interesting project, it may not be the kind of project
we would recommend to others. In some ways, parallelization ended up
being a smaller part of the project than it should have been. It
probably would have made more sense to just stick to one voting method,
probably STV, and working solely on interesting distribution problems
like security and fault tolerance. Having four different voting systems
ended up creating too much work that did not really build up to anything
big.

In terms of future work, certainly one could add more voting systems,
but most of that task would probably be implementing the serial version.
Obviously the problem of taking the system online remains, but that is
probably a relatively small task. Having a working client-server model
as part of that could open up a lot more interesting possibilities,
however, such as running nodes in entirely different locations and
working on securing communication, or making each node more independent
and removing the dependence on having a “root” node, though these are
problems that apply to more or less any client-server system. There are
interesting possibilities in our project, but probably not anything
interesting enough to warrant starting with our codebase rather than
starting from scratch with a smaller serial problem.
