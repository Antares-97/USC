from flask import Flask, jsonify, request
import googlenews

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 10


@app.after_request
def after_request(resp):
    resp.headers["Cache-Control"] = "no-store"
    resp.headers["Pragma"] = "no-cache"
    return resp


@app.route('/', methods=['POST', 'GET'])
def index():
    if request.method == "GET":
        return app.send_static_file('index.html')


@app.route('/headlines', methods=['GET', 'POST'])
def headlines():
    if request.method == "GET":
        stopwords = 'static/stopwords_en.txt'
        titles = googlenews.getTopLines(zipflag=False, nullflag=False)
        wordcloud = googlenews.wordfrequency(stopwords, titles)
        topHeadLines = googlenews.getTopLines(top=5)
        cnnHeadLines = googlenews.getTopLines(sources='cnn', top=4)
        foxHeadLines = googlenews.getTopLines(sources='fox-news', top=4)
        return jsonify(wordCloud=wordcloud, top=topHeadLines, cnn=cnnHeadLines, fox=foxHeadLines)


@app.route('/sources/<string:category>', methods=['GET', 'POST'])
def sources(category):
    if request.method == "GET":
        sources = googlenews.getsources(category=category)
        return jsonify(sources=sources)


@app.route('/search/<string:q>/<string:sources>/<string:From>/<string:To>', methods=['GET', 'POST'])
def search(q, sources, From, To):
    if request.method == "GET":
        try:
            result = googlenews.search(q=q, sources=sources, From=From, To=To)
            return jsonify(result=result, status='ok')
        except Exception as e:
            message = str(e)
            message = message.replace('\'', '"')
            return jsonify(result=message, status='error')


if __name__ == "__main__":
    app.run(debug=True)
