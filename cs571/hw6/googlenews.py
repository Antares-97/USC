from newsapi import NewsApiClient

myapikey = '747bbdc2e200478d8e838b7dc969d6e8'
newsapiclient = NewsApiClient(api_key=myapikey)


def getTopLines(sources='cnn,fox-news,the-verge,bbc-news,'
                        'bloomberg,cbs-news,google-news,time,abc-news',
                top=100, zipflag=True, nullflag=True):
    topheadlines = newsapiclient.get_top_headlines(page_size=100, sources=sources)
    articles = topheadlines['articles']
    result = []
    title = []

    limit = 0
    for i in range(len(articles)):
        news = articles[i]
        if nullflag and None in [news['author'], news['title'], news['description'],
                    news['url'], news['urlToImage'], news['publishedAt'],
                    news['source']['name'], news['content']]:
            continue
        else:
            limit += 1
            result.append(news)
            title.append(news['title'])
            if limit >= top:
                break

    if zipflag:
        return result
    else:
        return title


def str_transform(string):
    string = string.lower()
    string = string.replace("-", " ")
    string = string.replace("'", " ")
    string = string.replace(":", " ")
    string = string.replace(".", " ")
    string = string.replace('"', ' ')
    string = string.replace('’', ' ')
    string = string.replace(',', ' ')
    string = string.replace('.', ' ')
    return string


def wordfrequency(stopwordlist, titles):
    readfile = open(stopwordlist, 'r')
    lines = readfile.readlines()
    stopwords = [line.strip('\n') for line in lines]
    worddict={}
    for title in titles:
        title = str_transform(title)
        title = title.split()
        for word in title:
            if word in stopwords:
                continue
            elif word in worddict:
                worddict[word] += 1
            else:
                worddict[word] = 1
    worddict = sorted(worddict.items(), key=lambda x: x[1], reverse=True)
    wordcloud = [{'word':d[0], 'size':str(d[1])} for d in worddict[:30]]
    return wordcloud


def getsources(category, language='en', country='us'):
    classes = ['business', 'entertainment', 'general', 'health', 'science', 'sports', 'technology']
    if(category in classes):
        sources = newsapiclient.get_sources(category=category, language=language, country=country)
    else:
        sources = newsapiclient.get_sources(language=language, country=country)
    return sources['sources']


def search(q, sources, From, To, sortBy='publishedAt', top=15):
    if sources == 'all':
        sources = 'cnn,fox-news,the-verge,bbc-news,' \
                  'bloomberg,cbs-news,google-news,time,abc-news'

    everything = newsapiclient.get_everything(q=q, sources=sources, from_param=From,
                                              to=To, sort_by=sortBy, page_size=30)
    articles = everything['articles']
    result = []
    limit = 0
    for i in range(len(articles)):
        news = articles[i]
        if None in [news['author'], news['title'], news['description'],
                    news['url'], news['urlToImage'], news['publishedAt'],
                    news['source']['name'], news['content']]:
            continue
        else:
            limit += 1
            result.append(news)
            if limit >= top:
                break
    return result
    # at most 30 days ago from current date
