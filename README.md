# Ballon Météo

Ce projet est réalisé dans le cadre de l'enseignement de L1 de Communication sans fil.

# Description du projet:

Le projet que nous réalisons est un ballon météo. C'est dans la nacelle que se situe notre conception. Nos capteurs récupèrent des données météorologiques que nous transmettons à l'aide du protocole de communication LoRa vers un serveur d'application qui les récupère et les tweet.  

# Hardware:
Nous utilisons le matériel suivant:
- UCA Board et son arduino mini 
- Arduino UNO
- Sparkfun Weathershield
- LiPo rider pro
- Cellule solaire
- Batterie 400mAh
- Anémomètre

# Software:
Pour toute la gestion des capteurs et l'initialisation de la communication, c'est le langage arduino qui est utilisé. La communication sans fil est assurée par le protocole LoRa, le serveur d'application est géré par Node-Red qui renvoie nos données sur Twitter. Enfin nous utilisons InfluxdB comme base de données. 


# Que faisons nous concrétement ? 

L'arduino UNO se charge d'acquérir les données que récolte le Sparkfun Weathershield. Le UNO transmet alors par liaison filaire ces informations à l'arduino mini de l'UCA Board et celle-ci se charge de transmettre par la méthode d'activation OTAA une chaîne de caractère, répondant à un modèle définit par nos soins. Les informations sont envoyées vers une Gateway LoRa (ne faisant pas partie du projet) qui se charge de faire transiter cette chaîne vers un serveur d'application. Sur celui-ci, qui est géré par Node-Red, à l'aide des différents noeuds disponibles et ajoutable, les données sont récupérées, filtrées et envoyées vers Twitter pour être mises dans un tweet. Parallèlement à leur envoie, ces mêmes données sont stockées dans une base de données qui est gérée par InfluxdB. 
