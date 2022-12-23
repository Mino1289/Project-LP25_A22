# Projet de C LP25

## Contexte

Le contexte de ce projet est le développement d'un analyseur de fichiers texte pour en tirer des données. Le projet s'inspire du fonctionnement d'un _mapper/reducer_ dont le principe est d'alterner une phase de traitements parallèles (par exemple la lecture et l'analyse individuelle des fichiers) avec une phase de traitements séquentiels (par exemple, la concaténation des résultats individuels).

Les traitements parallèles peuvent être effectués simultanément sur plusieurs coeurs d'un ordinateur, alors que le traitement séquentiel doit passer par un seul processus.

## Fonctionnement schématique

Le projet fonctionne suivant plusieurs étapes :

- Lecture de la configuration du traitement : cette phase permet de définir les sources de données, les fichiers qui contiendront les résultats, éventuellement les traitements à effectuer. À partir de cette lecture, le programme peut construire une liste des tâches.
- Exécution de la liste des tâches : les tâches du _mapper_ vont commencer à être lancées, avec une limite de tâches simultanées basée sur le nombre de processeurs de la machine.
- Exécution du _reducer_ : à l'issue de l'exécution de **tous** les _mapper_, le _reducer_ va lire les résultats individuels et les assembler dans un résultat global.

Plusieurs séquences de _mapper/reducer_ peuvent être enchaînées. Ce sera le cas de ce projet.

Le reste de ce README définit plus en détails les éléments permettant d'accomplir les tâches mentionnées ci dessus.

## Cas d'application

Vous utiliserez comme source de données un corpus de 517401 e-mails de la société Enron. L'analyse des mails portera sur la fréquence des échanges de mails entre personnels. Pour cela, vous analyserez les entêtes de mails, notamment les champs **From**, **To**, **Cc** et **Bcc**.

Ce corpus de données est disponible à [l'URL suivante](https://www.cs.cmu.edu/~./enron/enron_mail_20150507.tar.gz)

Les données qui vous sont fournies le sont dans une arborescence comme suit :
```bash
maildir
├── allen-p
|   ├── all_documents
|   |   ├── 1.  # Fichier contenant un mail
|   |   ├── 2.  # Fichier contenant un autre mail
|   |   ├── ... # etc.
|   ├── contacts
|   ├── ...
├── arnold-j
|   ├── ...
├── ...
└── zufferli-j
```
Cette arborescence comporte 150 répertoires d'utilisateurs. Chaque répertoire a une structure interne qui ne nous intéresse pas ici (on y cherchera tous les fichiers récursivement, même s'il y a plusieurs niveaux de dossiers sous un utilisateur).

L'analyse consiste en plusieurs étapes :

- Lecture de la configuration, qui permet de déterminer les paramètres nécessaires au fonctionnement du programme
- Initialisation : pour les méthodes reposant sur des processus persistants
- Lancement des mappers pour construire la liste des fichiers à analyser : chaque mapper liste les fichiers de son répertoire à traiter (correspondant à un utilisateur, cf. ci dessous) et écrit cette liste dans son fichier de sortie.
- Lancement du reducer qui concatène la liste des fichiers : une fois tous les mappers de l'étape qui précède effectués, le reducer concatène les contenus des fichiers dans un seul fichier.
- Lancement des mappers qui analysent les fichiers de la liste : chaque mapper analyse un fichier de la liste générée à l'étape qui précède.
- Lancement du reducer qui concatène les résultats : pour gagner du temps et éviter l'écriture de plus de 500000 fichiers temporaires, les mappers écriront directement dans le fichier de résultat, qui sera verrouillé le temps de l'écriture. Puis le reducer lira les résultats complets pour produire un fichier avec les liens entre les utilisateurs.

### Lecture de la configuration

La lecture de la configuration permet de déterminer les éléments suivants :

- Le chemin vers les données à analyser (le répertoire `maildir` des e-mails d'Enron)
- Le chemin vers les données temporaires (il s'agit d'un dossier)
- Le chemin vers le fichier de résultat : c'est un chemin vers un fichier qui sera créé ou écrasé. Le chemin menant à ce fichier doit préalablement exister (par exemple, si le chemin vers le fichier de sortie est `/home/users/flassabe/LP25/Projet/resultats.out`, le chemin `/home/users/flassabe/LP25/Projet/` doit exister, et `Projet` doit être un répertoire)
- L'affichage ou non des informations complètes (appelé mode verbeux, ou _verbose_ en Anglais)
- Le nombre de tâches par thread de la machine (compris entre 1 et 10)

### Initialisation

Dans cette étape, les processus persistants, la file de messages, et les FIFOs sont créés, pour les méthodes qui se basent sur ces types de processus.

### Mapper de listage des fichiers

Dans cette étape, le processus père, qui est l'orchestrateur de l'analyse, va parcourir le répertoire racine des mails (`maildir`), et déléguer le listage de tous les fichiers contenus dans un des sous-répertoires à ses fils. Par exemple, un fils va traiter l'utilisateur (donc le répertoire) `allen-p`, un autre traitera le dossier suivant, et ainsi de suite. Le nombre de mappers en exécution à un instant donné ne peut pas dépasser le nombre de threads de l'ordinateur, multiplié par le nombre de tâches par thread. Il faudra donc concevoir une logique pour lancer le maximum de tâches autorisé, puis n'en lancer de nouvelles que quand des anciennes se terminent.

Cette étape crée, dans le répertoire temporaire, un fichier portant le même nom que l'utilisateur en cours de traitement, et contenant la liste de tous les fichiers trouvés récursivement dans ce répertoire. On y trouvera un contenu similaire à :
```text
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/190.
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/102.
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/89.
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/63.
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/5.
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/209.
/home/flassabe/Téléchargements/enron_mail_20150507/maildir/ermis-f/notes_inbox/121.
...
```

### Reducer de listage des fichiers

Le reducer de listage des fichiers doit attendre que tous les mappers se soient terminés et que les données aients effectivement été écrites sur le disque (il faudra donc écrire une fonction pour synchroniser les données écrites sur le support de stockage).

Une fois la synchronisation terminée, le reducer crée dans le répertoire temporaire un fichier nommé `step1_output`. Puis il concatène l'ensemble des fichiers issus de la première exécution des mappers dans ce fichier. Les fichiers créés par les mappers sont ensuite supprimés.

`step1_output` a la même structure que les fichiers créés par les mappers, et il doit contenir 517401 lignes (c'est le nombre de fichiers qui seront analysés à l'étape suivante)

### Mapper d'analyse du contenu des mails

Cette étape consiste à lire le fichier `step1_output` ligne par ligne (chaque ligne contenant le chemin vers un fichier contenant un e-mail), et à faire analyser le contenu de ce fichier par un worker. Le worker va chercher les champs **From**, **To**, **Cc** et **Bcc** et en extraire les adresses e-mail qui y figurent. Le champ **From** devra être le premier à être analysé. En effet, l'adresse e-mail qui y figure devra être la première à figurer sur la ligne du fichier temporaire de sortie, suivie de tous les destinataires. Ensuite, les champs de destinataires (normaux, en copie, et cachés) seront également extraits. Si aucun de ces champs n'existe (attention, n'utilisez que **To**, **Cc** et **Bcc**, et pas **X-To**, etc.), aucun résultat ne sera produit. Dans le cas contraire, une ligne est ajoutée au fichier temporaire (nommé `step2_output`), sous la forme :
```text
e-mail.expediteur@mail.domain destinataire1@dest.domain1 ... destinataireN@dest.domainN
```
Cet ajout est fait avec un verrou sur le fichier durant l'écriture.

Comme pour le premier mapper, il ne pourra pas y avoir plus de processus en exécution que le nombre de threads de l'ordinateur, multiplié par le nombre de tâches par thread.

### Reducer d'analyse

Le reducer d'analyse va lire le fichier `step2_output` et produire un fichier avec les informations condensées : il n'y aura qu'une ligne par adresse d'expédition, suivie de l'ensemble des adresses destinataires avec le nombre d'occurrences pour chacune. Par exemple un fichier fictif `step2_output` contenant les enregistrements suivants :
```text
toto@enron.com bob@enron.com alice@enron.com
bibi@enron.com bob@enron.com
bob@enron.com alice@enron.com bill@enron.com
toto@enron.com alice@enron.com
bob@enron.com potus@whitehouse.gov.us
```
Donnera après regroupement des données :
```text
toto@enron.com 1:bob@enron.com 2:alice@enron.com
bibi@enron.com 1:bob@enron.com
bob@enron.com 1:alice@enron.com 1:bill@enron.com 1:potus@whitehouse.gov.us
```
Notez bien que chaque émetteur n'apparaît plus qu'une fois, et que les destinataires sont précédés de leur nombre d'occurrences dans les e-mail de l'émetteur. Le fichier final est écrit dans le fichier défini par la variable de configuration `output_file`.

## Implémentations pratiques

Dans ce projet, il vous sera demandé d'implémenter 3 manières de gérer les processus. L'objectif premier est de ne pas dépasser une limite donnée de processus (basée sur le nombre de cores de votre machine). Les 3 manières sont les suivantes :

- Lancement des processus depuis la boucle qui les alimente, avec attente de la fin d'un processus si le nombre maximal est atteint. Dans le reste de ce document, cette manière est nommée **lancement direct**.
- Lancement du nombre maximal de processus au début du programme, et communication entre le processus du `main` et les processus _worker_ à l'aide de fichiers FIFO. Dans la suite de ce document, cette manière est nommée **orchestration par FIFO**.
- Lancement du nombre maximal de processus au début du programme, et communication entre le processus du `main` et les _workers_ à l'aide d'une file de messages (_messages queue_). Dans la suite de ce document, cette manière est nommée **orchestration par MQ**.

### Lancement direct

Pour le lancement direct, le processus principal va parcourir la liste des éléments à traiter en parallèle, soit :

- Les dossiers directement sous `maildir` dans la première étape,
- les fichiers listés dans le fichier produit par le _reducer_ de l'étape 1 dans la seconde étape.

Pour chaque élément à traiter, il crée une tâche du type souhaité (tâche de traitement de dossier en première étape, tâche de traitement de fichier en seconde étape) avant de tester s'il peut l'envoyer dans un processus.

Cette vérification est simple : après avoir créé la tâche, le processus principal vérifie si son nombre d'enfants est égal au nombre de processus maximum paramétré. Si c'est le cas, il doit attendre avec la fonction `wait` qu'un des enfants se termine. Puis il peut lancer la tâche. Si le nombre actuel d'enfants est déjà inférieur à la cible, il peut lancer immédiatement la tâche sans attendre. Soyez vigilants à la bonne tenue de votre compteur de processus enfants lancés.

### Orchestration par FIFO

Dans l'orchestration par FIFO, les processus enfants sont créés avant de commencer les traitements, et ils attendent des commandes lues sur une FIFO, et informent de la terminaison de leurs tâches via une autre FIFO. Les FIFO sont nommées **fifo-in-_X_** et **fifo-out-_X_** avec X prenant ses valeurs entre 0 et le nombre cible de processus - 1. Vous aurez donc autant de paires de FIFO in/out que de processus cibles.

Il est donc nécessaire de commencer le programme par la création d'autant de FIFO entrantes et sortantes qu'il n'y a de processus enfant à créer, puis de créer ces enfants avant d'ouvrir les FIFO dans le mode souhaité selon le processus.

Le parent peut ensuite envoyer les tâches aux enfants en écrivant les structures des tâches dans les FIFO, que les enfants vont ensuite lire et exécuter. À la terminaison d'une tâche, un enfant va signaler au parent dans sa FIFO sortante qu'il est en attente d'une tâche.

### Orchestration par MQ

Dans l'orchestration par file de message, les processus enfants sont également créés avant de commencer les traitements, et ils attendent des commandes sur une file de messages. Les messages sont adressés avec un identifiant de topic qui correspond au numéro de PID du worker dans le sens parent -> worker, et avec l'identifiant 1 (c'est l'identifiant du processus `init` ou `systemd`, selon la distribution, qui ne sera donc pas utilisé par un des processus du projet, ce qui fait qu'il n'y a aucun risque d'ambiguité) pour les messages des workers vers le parent.

# Implémentation

Certaines valeurs sont fixées par des définitions dans le fichier `global_defs.h`. Il contient notamment la définition de la longueur maximale des chaînes de caractères utilisées dans le programme, ainsi que la définition de la structure d'une tâche sans spécificité.

```c
#define STR_MAX_LEN 1024

typedef struct _task {
    void (* task_callback)(struct _task *);
    char argument[2*STR_MAX_LEN]; // Can be extended based on actual task
} task_t;
```

## Fonctions utilitaires

Un certain nombre de fonctions utilitaires seront utilisées (voir les fichiers `utility.h` et `utility.c`) :
```c
char *concat_path(char *prefix, char *suffix, char *full_path);
bool directory_exists(char *path);
bool path_to_file_exists(char *path);
```
Vous devrez les implémenter.

## Construction de la configuration

Le programme est configurable par plusieurs canaux :

- Certains paramètres de configuration ont des valeurs par défaut. Celles-ci doivent être explicitement initialisées au lancement du programme.
- D'autres paramètres (comme le chemin vers la source de données) sont requis mais ne peuvent pas avoir de valeur par défaut. En leur absence, le programme devra quitter avec une valeur de retour différente de zéro.

Les valeurs des paramètres sont fixées dans l'ordre par :

- les valeurs par défaut
- les valeurs du fichier de configuration
- les valeurs transmises par les options d'appel du programme

Cela signifie que, si l'option _verbose_ (affichage d'informations complémentaires sur le fonctionnement du programme) est _off_ par défaut, qu'elle est _on_ dans le fichier de configuration, et qu'elle est _off_ dans les options de la ligne de commande d'appel du programme, elle sera effectivement _off_ pour l'exécution du programme. En effet, le _on_ du fichier de configuration écrase le _off_ par défaut, mais le _off_ des paramètres de la ligne de commande écrase le _on_ du fichier.

L'analyse des options de la ligne de commande doit se faire avec l'API `getopt`. Le code sera localisé dans les fichiers `configuration.h` et `configuration.c`. La taille des chaînes de caractères de la configuration est limitée à `STR_MAX_LEN` défini dans `global_defs.h`.

Le tableau ci-dessous décrit les variables et leurs éventuelles valeurs par défaut.

| Nom | Flag CLI | Type | Définition | Défaut |
| --- | ---- | ---- | ---------- | ------ |
| data_path | -d | `char[]` | Dossier des données source | `""` |
| output_file | -o | `char[]` | Fichier de résultat final | `""` |
| temporary_directory | -t | `char[]` | Chemin vers le dossier des données temporaires | `""` |
| is_verbose | -v | `bool` | Commutateur de verbosité | `false` |
| cpu_core_multiplier | -n | `uint8_t` | nombre de processus par core | `2` |
| | -f | `char[]` | Chemin vers le fichier de config | non inclus dans `configuration_t` |

`Nom` est le nom de l'option dans le fichier de configuration, `Flag CLI` est le nom de l'option pouvant être passée au programme par la CLI.
Référez vous au TP6 pour plus de détails sur l'analyse des paramètres.

Vous utiliserez la structure suivante pour analyser la configuration :
```c
typedef struct {
    char data_path[STR_MAX_LEN];
    char temporary_directory[STR_MAX_LEN];
    char output_file[STR_MAX_LEN];
    bool is_verbose;
    uint8_t cpu_core_multiplier;
} configuration_t;

configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc);
configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file);
void display_configuration(configuration_t *configuration);
bool is_configuration_valid(configuration_t *configuration);
```

Dans le fichier de code `configuration.c`, vous trouverez également 3 fonctions utilitaires à écrire :
```c
char *skip_spaces(char *str);
char *check_equal(char *str);
char *get_word(char *source, char *target);
```
Le fonctionnement des ces fonctions est décrit par les commentaires doxygen dans le fichier C.

Le fichier de configuration a le format suivant (les champs ne sont pas obligatoires, cet exemple montre juste un fichier de configuration définissant toutes les options) :
```text
is_verbose = no
data_path = /home/flassabe/Téléchargements/enron_mail_20150507/maildir
temporary_directory = /home/flassabe/Téléchargements/enron_mail_20150507/temp
output_file = /home/flassabe/Téléchargements/enron_mail_20150507/result.out
cpu_core_multiplier = 3
```

## Initialisation du lancement

L'initilisation varie selon la méthode en cours d'utilisation.

### Lancement direct

Pour ce type de lancement, il n'y a rien à initialiser. Les données nécessaires à une tâche gérée par un processus sont copiées par le processus de création du nouveau processus, lors de l'appel à la fonction `fork`.

### Orchestration par FIFO

Ici, il sera nécessaire de réaliser les opérations suivantes pour chacun des `n` processus qui vont être utilisés :

- Créer les `n` FIFO entrantes (du point de vue du processus worker; ce sont les FIFO dans lesquelles le processus principal enverra les commandes à ses workers), nommées fifo-in-i où `i` vaut `0` à `n-1` (inclus)
- Créer les `n` FIFO sortantes  (du point de vue du processus worker; ce sont les FIFO desquelles le processus principal recevra l'indication de fin de tâche des workers), nommées fifo-out-i où `i` vaut `0` à `n-1` (inclus)
- Enfin, créer les `n` processus, qui vont chacun ouvrir en lecture (et sous forme de descripteur de fichier) leur FIFO d'entrée et de sortie. Le premier processus créé ouvrira les FIFO numérotées `0`, le second celles numérotées en `1`, etc. Utilisez pour cela le fait qu'un fils copie toutes les données du père lors de l'appel à la fonction  `fork` (dont la valeur du compteur de boucle avec lequel on compte les processus créés).

### Orchestration par MQ

L'initialisation consiste à créer la MQ depuis le père, puis à créer les `n` fils en leur passant en paramètre l'identifiant de la MQ, qu'ils pourront ensuite utiliser.

## Définition des tâches

Les tâches seront transmises sous forme d'une structure comportant un pointeur vers une structure contenant la fonction à traiter, ainsi que les paramètres de cette fonction. Ces types, affichés ci dessous, sont également visibles dans les fichiers `analysis.h` et `analysis.c`.
```c
typedef struct {
    void (* task_callback)(task_t *);
    char object_directory[STR_MAX_LEN];
    char temporary_directory[STR_MAX_LEN];
} directory_task_t;

typedef struct {
    void (* task_callback)(task_t *);
    char object_file[STR_MAX_LEN];
    char temporary_directory[STR_MAX_LEN];
} file_task_t;

void parse_dir(char *path, FILE *output_file);

void process_directory(task_t *task);
void process_file(task_t *task);
```
Vous aurez notamment à implémenter les fonctions qui y figurent.

## Lancement des tâches

Vous utiliserez la fonction `fork` pour lancer des processus. Le travail conceptuel principal du projet se trouve dans cette partie. Chaque méthode de parallélisation du calcul a ses propres spécificités.

### Lancement direct

Dans le lancement direct, vous tirerez parti du fait qu'avec `fork` le processus fils obtient une copie de la mémoire du processus père. En affectant au niveau du père des valeurs aux variables de chemin des dossiers ou fichiers (selon l'étape) en cours de traitement, le fils aura également un accès à ces données et pourra les traiter. Un exemple (hors du sujet du projet) pourrait être :

```c
#include <unistd.h>

int fibonacci(int v) {
	if (v == 0 || v == 1)
		return v;
	int n = 1, n_1 = 0, res;
	for (int i=2; i<=v; ++i) {
		res = n + n_1;
		n_1 = n;
		n = res;
	}
	return res;
}

int main() {
	for (int i=10; i>0; --i) {
		if (fork() == 0) {
			printf("Fibonacci(%d)=%d (process #%d)\n", i, fibonacci(i), getpid());
			exit(0);
		}
	}
	for (int i=0; i<10; ++i)
		wait(NULL); // Wait for all children to end
	return 0;
}
```

Ce programme produira une sortie similaire à :
```text
Fibonacci(10)=55 (process #51728)
Fibonacci(9)=34 (process #51729)
Fibonacci(8)=21 (process #51730)
Fibonacci(7)=13 (process #51731)
Fibonacci(6)=8 (process #51732)
Fibonacci(5)=5 (process #51733)
Fibonacci(4)=3 (process #51734)
Fibonacci(3)=2 (process #51735)
Fibonacci(2)=1 (process #51736)
Fibonacci(1)=1 (process #51737)
```

Vous devrez `fork`er autant de fois qu'il n'y a de tâches (150 fois pour la liste des fichiers pour chaque utilisateur, 517401 fois pour l'analyse du contenu des mails), en vous assurant d'attendre que des processus se terminent avant d'en relancer pour vous maintenir au nombre cible de processus simultanés (sans quoi, vous risquez de causer un effondrement des performances de votre système)

### Orchestration par FIFO

Vous devrez remplir pour chaque tâche une structure de type `directory_task_t` ou `file_task_t` avec les informations relatives à la tâche (la fonction à exécuter dans le callback de la tâche, et les chemins du répertoire/fichier à traiter, ainsi que le chemin vers le fichier de résultat intermédiaire, c'est-à-dire `step1_output` ou `step2_output`). Vous transmettrez ensuite ces tâches en écrivant la structure dans la FIFO entrante correspondant au processus auquel envoyer la tâche, et en vous assurant de ne pas envoyer une tâche à un processus déjà en cours de traitement (les processus disposent d'une FIFO sortante lue par le père pour avertir ce dernier de la fin de traitement d'une tâche et lui permettre d'envoyer une nouvelle tâche le cas échéant).

Le père devra quant à lui utiliser `select` pour écouter en même temps sur toutes les FIFO sortantes et mettre à jour le nombre de tâches en cours et avancer dans les tâches à envoyer.

### Orchestration par MQ

L'orchestration par MQ est similaire à celle par FIFO, excepté dans les détails d'implémentation (par exemple, le père n'a pas besoin de `select` pour écouter les workers car les messages sont envoyés dans la file de messages et reçus par un seul et unique descripteur). Les identifiants de sujet (`message.mtype`) à lire pour les différents processus sont :

- 1 pour le père
- le PID du fils pour chaque fils.

## Collecte et concaténation des résultats

### Phase 1

Pour concaténer la liste des fichiers, le reducer va lister les dossiers du répertoire source de données (150 répertoires d'utilisateur) et se servir de leurs noms pour ouvrir les fichiers nommés pareil dans le répertoire temporaire. Le contenu de chaque fichier est ensuite lu pour être écrit dans la liste complète, localisée dans le répertoire temporaire, et nommé `step1_output`.

### Phase 2

Dans la phase 2, les données sont déjà dans un seul fichier, `step2_output`, mais avec une ligne par mail. Pour regrouper les statistiques par e-mail source, vous devrez parcourir le fichier, extraire la première adresse, chercher si elle existe dans la liste des adresses, l'ajouter si ce n'est pas le cas. Puis, vous mettrez à jour les valeurs liées à cette adresse : si une adresse de destination existe déjà, vous incrémenterez son compteur d'occurrences, dans le cas contraire, vous ajouterez la destination avec une occurrence égale à 1. Les listes d'adresses sources, et de destinataires seront ordonnées par ordre alphabétique d'adresse mail. Vous utiliserez une structure de listes chaînées du type ci dessous :

```c
typedef struct _recipient {
	char recipient_address[STR_MAX_LEN];
	uint32_t occurrences;
	struct _recipient *prev;
	struct _recipient *next;
} recipient_t;

typedef struct _sender {
	char recipient_address[STR_MAX_LEN];
	recipient_t *head; // Head of recipient list
	recipient_t *tail; // Tail of recipient list
	struct _sender *prev;
	struct _sender *next;
} sender_t;
```

Pour finir, vous écrirez (comme défini au début de ce README) le contenu des listes dans le fichier défini par le paramètre `output_file` du programme.

## Rendus du projet

Le projet sera évalué sur la base de 3 éléments principaux :

- la soutenance (25% de la note de projet), une présentation de 10 minutes de tout le groupe (en équilibrant les temps de parole) dans laquelle :
	- vous rappellerez le contexte du projet, les grandes lignes de son fonctionnement (2 minutes)
	- vous expliquerez les choix que vous avez eu à faire et pourquoi (3 minutes)
	- vous présenterez les difficultés les plus importantes rencontrées et vos analyse à ce sujet (2 minutes)
	- vous proposerez un RETEX (retour d'expérience) dans lequel vous répondrez du mieux que possible à la question suivante : "_fort(e)s de notre expérience sur le projet, que ferions-nous différemment si nous devions le recommencer aujourd'hui ?_"
	- vous ferez une brève conclusion (1 minute)
	- vous répondrez ensuite à des questions posées par les enseignants pendant une dizaine de minutes.
- le rapport (25% de la note de projet), dont un canvas vous sera fourni plus tard dans le semestre et dans lequel vous devrez détailler plus précisément ce qui sera présenté à la soutenance, ainsi que des résultats quantitatifs de votre travail.
- le code (50% de la note de projet), sous forme d'une archive contenant l'ensemble des éléments nécessaires pour être compilé (donc un Makefile) ou d'un lien vers un dépôt git accessible sans authentification. Tous les éléments suivants entrent dans les critères de notation du code :
	- La qualité "visuelle" du code :
		- indentation
		- choix des noms de variables et fonctions
		- etc.
	- la qualité et la pertinence des commentaires
	- la qualité de l'implémentation, notamment la capacité à ne pas crasher
	- l'absence de fuites mémoire (testée avec `valgrind`)
	- le taux d'objectifs atteints (si le projet est incomplet)

Attention : le code doit compiler sous Linux ! Un code non compatible avec un système Linux sera pénalisé de 5 points sur 20.

Les dates prévisionnelles des rendus sont les suivantes :

- Soutenances pendant les TP de la semaine de rentrée
- Rapport à envoyer avant le vendredi 06/01/2023, 23:59:59 GMT+1 (pénalité sur la note du rapport de 2^n-1 points pour n jours de retard)
- Code : 2 dates de rendu
	- 1 préversion avant les vacances (vendredi 16/12/2022, à 23:59:59 GMT+1)
	- 1 version finale après les soutenances (vendredi 06/01/2023, à 23:59:59 GMT+1)
