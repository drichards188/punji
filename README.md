# NOT UPDATED OR MONITORED

# PRE ALPHA

# punji

A email open relay is a unsecured MSA server that is used to send email, usually spam or other malicous content. Open relays are detected by crawlers that run 24/7.

A honeypot is a service that mimicks another service to lure attackers. They can be used for active defense, intelligence gathering, and diversion.

Punji is a honeypot that mimicks an email open relay. It is open to being crawled(it responds to ehlo and helo commands) and responds when email is attempted to send through it.

In reality no email is sent. But the location, content and actions of the attackers and spammers is recorded. The end goal is to compile actionable threat intelligence. 

# Usage

Punji is the honeypot that is deployed to the global servers. Punjicc is the command and control server to be run from one location. To monitor global trends deploy Punji to each major country through a service like AWS. In my experience south Asian locations receive the most traffic. But American and British locations receive the most sophisticated content. Almost all of the email is in English regardless of location, I'm not sure why. 

I have been using Kibana to graphically view the data that is stored in Mongo to extrapulate patterns.

Feel free to use Punji as long as you don't use it for revenue. Feel free to expand on Punji. If you want to use my code in your own project you need my written permission.
