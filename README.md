# PP2-BattleSim-MI
Periode 2 Battle Sim Project

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/58c2d9ea774341fb812b4c3189f63555)](https://www.codacy.com/manual/kevin.celinski/PP2-BattleSim-MI?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Altair115/PP2-BattleSim-MI&amp;utm_campaign=Badge_Grade)

Requirements:
Algoritmiek (1+):
-------------------
- Ten minste 3 algoritmische verbeteringen (3)
  - Je hebt met behulp van een algoritmische oplossing de algoritmische complexiteit van de code op tenminste drie plaatsen verbeterd.
- Algoritmen zorgen voor speedup (2)
  - Omdat je met een big O analyse of profiling (de grootste) bottlenecks geïdentificeerd hebt zorgen de algoritmische verbeteringen voor een speedup.
- Verslag met O analyse (2)
  - Voeg een (klein) verslag toe van de verbeteringen die je hebt uitgevoerd. Zet hierin de voor en na big O en/of profiling resultaten.
- Code kwaliteit (1)
  - Zorg ervoor dat je code leesbaar blijft. OOP is niet een vereiste, maar spaghetti code is voor niemand wenselijk.
- Exceptionele speedup (1)
  - Voor de studenten met exceptionele speedup waarden is er een bonus punt.
- Bonus (1)
  - Voor exceptionele toevoegingen aan het project.
-------------------

Concurrent (1+):
-------------------
- Gebruikt RAII guard (e.g. std::lock_guard) i.p.v. pure mutex (1)
  - Pure mutex is error prone, maak gebruik van de RAII wrapper versies.
- Gebruikt threadpool (1)
  - Je maakt gebruik van een threadpool om de overhead van individuele threads aanmaken te verminderen.
- Threads doen niet (te veel) onnodig werk (1)
  - Waar mogelijk stop je threads zodat ze niet onnodig werk verrichten.
- Verslag met work, span, speedup analyse (2)
  - In een (klein) verslagje documenteer je de concurrent optimalisaties die uitgevoerd zijn. Meld hierbij ook de work en span.
- Effectieve concurrency (2)
  - Omdat je met een big O analyse of profiling (de grootste) bottlenecks geïdentificeerd hebt zorgen de concurrency verbeteringen voor een speedup.
- Code kwaliteit (1)
  - Zorg ervoor dat je code leesbaar blijft. OOP is niet een vereiste, maar spaghetti code is voor niemand wenselijk.
- Exceptionele speedup (1)
  - Voor de studenten met exceptionele speedup waarden is er een bonus punt.
- Bonus (1)
  - Voor exceptionele toevoegingen aan het project.
--------------------
