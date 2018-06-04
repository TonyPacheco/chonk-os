# MiniOS

An instructional operating system, which happens to run on microncontrollers (MCUs) 

<p align="center">
  <img src="https://github.com/rromanotero/minios/blob/master/logo.png" width="290"/>
  <p align="center">It's supposed to be a "wireless" mug! (because coffee is how it got written)</p>
</p>


# Origins

MiniOS was originally written as an instructional OS for teaching Operating Systems Labs at the University of Northern BC. This is more or less explained [here](http://embedntks.com/wp-content/uploads/2016/08/MiniOS-Defense-Slides.pptx). (Please note the version initially uploaded here is not exactly the same as the one referenced in the slides)


# Use

The OS is intended to be delivered together with a book that shows step by step how to build MiniOS from scratch. So you need to have the book to make sense of this code.

How to get the book? Well, the book for the most part complete, but my co-author (Dr. Alex Aravind) and me have been busy and are still working out the final details. Should you want a copy for academic purposes or just for yourself to learn, feel free to send me an e-mail!


# Can a microcontroller be used to teach Operating Systems?

Yes (arguably). For three years we offered OS Labs for Computer Science students at the University of Northern BC. If you're really interested read [Chapter 3 (Rationale)](http://embedntks.com/wp-content/uploads/2016/08/MiniOS-Thesis.pdf). 


# MiniOS Apps

One important aspect of MiniOS is that it ca run actual "apps". Separately-written and separately-deployable apps. Like in a general purpose computer system.


# What is it used for (other than instruction)?

One of the design goals of MiniOS was that it should be "functional". Meaning, one should be able to use it to build stuff. So even though MiniOS is very minimal, and you could say "it's missing parts", you can still use it. It works! The parts that are written do not randomly break. As long as you are aware of certain constraints (e.g. don't go beyond the max number of processes; or tell the kernel to run something that isn't an app; that'll break it!).


What exactly can you build? Still exploring this. One example is MCU robots, such as [this one](https://www.youtube.com/watch?v=Tp8fJ_0Ap_s). 


# On what platforms does it run?

Currently the SAM4S Xplained Pro.


# Compiling Source code

Source code is an Atmel Studio Project.


# Running Sample Apps

Samples apps (.bin) are in /sample_apps/apps_known_to_work. Run them using the console. As shown [here](https://youtu.be/xwV5s2CTYqg) and [here](https://youtu.be/HzgY5F7Pba4)

To write your own, you have to use the template in /sample_apps/atmel_studio_template (also an Atmel Studio Project)


# Publication and Thesis

Rafael Roman Otero. MiniOS: An Instructional Platform for Teaching Operating Systems Labs (Master's Thesis).  University of Northern British Columbia. 2016. [PDF here](http://embedntks.com/wp-content/uploads/2016/08/MiniOS-Thesis.pdf)

Rafael Roman Otero and Alex A. Aravind. 2015. MiniOS: An Instructional Platform for Teaching Operating Systems Projects. In Proceedings of the 46th ACM Technical Symposium on Computer Science Education (SIGCSE ’15). ACM, New York, NY, USA, 430-435. [PDF here](http://embedntks.com/datastructs/papers/minios%20paper%20%28published%20version%29.pdf)

