const guardian_api_key = '2e294fb6-60a7-497d-a1c0-653035370a6d';
const Guardian_Home_URL = 'https://content.guardianapis.com/search?api-key=' + guardian_api_key +
                          '&section=(sport|business|technology|politics)&show-blocks=all';
const guardianImage = "https://assets.guim.co.uk/images/eada8aa27c12fe2d5afa3a89d3fbae0d/fallback-logo.png";

/**
 * @return {string}
 */
function Guardian_Section_URL(sectionName) {
  return ('https://content.guardianapis.com/' + sectionName +
      '?api-key=' + guardian_api_key +'&show-blocks=all');
}


/**
 * @return {string}
 */
function Guardian_News_URL(newsID) {
  return ('https://content.guardianapis.com/' + newsID +
      '?api-key=' + guardian_api_key +'&show-blocks=all');
}


/**
 * @return {string}
 */
function Guardian_Query_URL(Keyword){
  return ('https://content.guardianapis.com/search?q=' +
      Keyword + '&api-key=' + guardian_api_key + '&show-blocks=all');
}


const nytimes_api_key = 'aheUXuNKJLOFtIiaH8TAPLH5vk3WYOq0';
const NYTimes_Home_URL =  'https://api.nytimes.com/svc/topstories/v2/home.json?api-key=' +
    nytimes_api_key;
const nytimesImage = "https://upload.wikimedia.org/wikipedia/commons/0/0e/Nytimes_hq.jpg";

/**
 * @return {string}
 */
function NYTimes_Section_URL(sectionName){
  return ('https://api.nytimes.com/svc/topstories/v2/'+
      sectionName + '.json?api-key=' + nytimes_api_key);
}


/**
 * @return {string}
 */
function NYTimes_News_URL(newsURL){
  return ('http://api.nytimes.com/svc/search/v2/articlesearch.json?fq=web_url:%22' +
      newsURL + '%22&api-key=' + nytimes_api_key);
}


/**
 * @return {string}
 */
function NYTimes_Query_URL(Keyword){
  return ('https://api.nytimes.com/svc/search/v2/articlesearch.json?q=' +
      Keyword + '&api-key=' + nytimes_api_key);
}


const bing_api_key = '1af9e7ee4435417aa7082b84c3abf263';

/**
 * @return {string}
 */
function AutoSuggest_URL(Keyword){
  return ('https://api.cognitive.microsoft.com/bing/v7.0/suggestions?q='
      + Keyword);
}


const express = require('express');
const app = express();
const XMLHttpRequest = require('xmlhttprequest').XMLHttpRequest;


app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "https://cs571-2270250947-hw8.wm.r.appspot.com"); // update to match the domain you will make the request from
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});


function getGuardianSection(newslist){
  let results = [];
  for(let i=0; i<newslist.length; i++){
    if(!newslist[i].hasOwnProperty('id') ||
        newslist[i]['id'] === "" ||
        !newslist[i].hasOwnProperty('sectionId') ||
        newslist[i]['sectionId'] === "" ||
        !newslist[i].hasOwnProperty('webTitle')||
        newslist[i]['webTitle'] === "" ||
        !newslist[i].hasOwnProperty('webUrl')||
        newslist[i]['webUrl'] === "" ||
        !newslist[i].hasOwnProperty('webPublicationDate') ||
        newslist[i]['webPublicationDate'] === "" ||
        !newslist[i]['blocks'].hasOwnProperty('body') ||
        newslist[i]['blocks']['body'][0]['bodyTextSummary'] === "" ||
        !newslist[i]['blocks']['body'][0].hasOwnProperty('bodyTextSummary') ||
        !newslist[i]['blocks'].hasOwnProperty('main') ||
        !newslist[i]['blocks']['main'].hasOwnProperty('elements')){
      continue;
    }
    let news = {};
    news.id = newslist[i]['id'];
    news.title = newslist[i]['webTitle'];
    news.url = newslist[i]['webUrl'];
    news.date = newslist[i]['webPublicationDate'].substring(0,10);
    news.description = newslist[i]['blocks']['body'][0]['bodyTextSummary'];
    news.section = newslist[i]['sectionId'];

    const array = newslist[i]['blocks']['main']['elements'];
    let images = [];
    for(let i=0; i<array.length; i++){
      if(array[i]['type'] === "image"){
        images = array[i]['assets'];
        break;
      }
    }
    if(images.length === 0 || images[images.length-1]['file'] === ""){
      news.image = guardianImage;
    }
    else{
      news.image = images[images.length-1]['file'];
    }

    results.push(news);
    if(results.length >= 10){
      break;
    }
  }
  return results;
}


app.get('/api/guardian/section/home', (req, res) => {
  const url = Guardian_Home_URL;
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const newslist = resp.response.results;
      res.send(JSON.stringify({ "results": getGuardianSection(newslist)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


app.get('/api/guardian/section/:Name', (req, res) => {
  const url = Guardian_Section_URL(req.params.Name);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const newslist = resp.response.results;
      res.send(JSON.stringify({ "results": getGuardianSection(newslist)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


function getGuardianNews(data){
  if( !data.hasOwnProperty('id') ||
      data['id'] === "" ||
      !data.hasOwnProperty('sectionId') ||
      data['sectionId'] === "" ||
      !data.hasOwnProperty('webTitle')||
      data['webTitle'] === "" ||
      !data.hasOwnProperty('webUrl')||
      data['webUrl'] === "" ||
      !data.hasOwnProperty('webPublicationDate') ||
      data['webPublicationDate'] === "" ||
      !data['blocks'].hasOwnProperty('body') ||
      data['blocks']['body'][0]['bodyTextSummary'] === "" ||
      !data['blocks']['body'][0].hasOwnProperty('bodyTextSummary') ||
      !data['blocks'].hasOwnProperty('main') ||
      !data['blocks']['main'].hasOwnProperty('elements')){
    return {};
  }

  let news = {};
  news.id = data['id'];
  news.title = data['webTitle'];
  news.url = data['webUrl'];
  news.date = data['webPublicationDate'].substring(0,10);
  news.description = data['blocks']['body'][0]['bodyTextSummary'];
  news.source = "GUARDIAN";
  news.section = data['sectionId'].toLowerCase();

  const array = data['blocks']['main']['elements'];
  let images = [];
  for(let i=0; i<array.length; i++){
    if(array[i]['type'] === "image"){
      images = array[i]['assets'];
      break;
    }
  }
  if(images.length === 0 || images[images.length-1]['file'] === ""){
    news.image = guardianImage;
  }
  else{
    news.image = images[images.length-1]['file'];
  }

  return news;
}


app.get('/api/guardian/news/', (req, res) => {
  const url = Guardian_News_URL(req.query.id);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const data = resp.response.content;
      res.send(JSON.stringify({ "data": getGuardianNews(data)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});

function getGuardianSection(newslist){
  let results = [];
  for(let i=0; i<newslist.length; i++){
    if(!newslist[i].hasOwnProperty('id') ||
        newslist[i]['id'] === "" ||
        !newslist[i].hasOwnProperty('sectionId') ||
        newslist[i]['sectionId'] === "" ||
        !newslist[i].hasOwnProperty('webTitle')||
        newslist[i]['webTitle'] === "" ||
        !newslist[i].hasOwnProperty('webUrl')||
        newslist[i]['webUrl'] === "" ||
        !newslist[i].hasOwnProperty('webPublicationDate') ||
        newslist[i]['webPublicationDate'] === "" ||
        !newslist[i]['blocks'].hasOwnProperty('body') ||
        newslist[i]['blocks']['body'][0]['bodyTextSummary'] === "" ||
        !newslist[i]['blocks']['body'][0].hasOwnProperty('bodyTextSummary') ||
        !newslist[i]['blocks'].hasOwnProperty('main') ||
        !newslist[i]['blocks']['main'].hasOwnProperty('elements')){
      continue;
    }
    let news = {};
    news.id = newslist[i]['id'];
    news.title = newslist[i]['webTitle'];
    news.url = newslist[i]['webUrl'];
    news.date = newslist[i]['webPublicationDate'].substring(0,10);
    news.description = newslist[i]['blocks']['body'][0]['bodyTextSummary'];
    news.section = newslist[i]['sectionId'].toLowerCase();
    news.source = "GUARDIAN";

    const array = newslist[i]['blocks']['main']['elements'];
    let images = [];
    for(let i=0; i<array.length; i++){
      if(array[i]['type'] === "image"){
        images = array[i]['assets'];
        break;
      }
    }
    if(images.length === 0 || images[images.length-1]['file'] === ""){
      news.image = guardianImage;
    }
    else{
      news.image = images[images.length-1]['file'];
    }

    results.push(news);
  }
  return results;
}

app.get('/api/guardian/query/', (req, res) => {
  const url = Guardian_Query_URL(req.query.keyword);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const newslist = resp.response.results;
      res.send(JSON.stringify({ "results": getGuardianSection(newslist)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


function getNYTimesSection(newslist){
  let results = [];
  for(let i=0; i<newslist.length; i++){
    if(!newslist[i].hasOwnProperty('section') ||
        newslist[i]['section'] === "" ||
        !newslist[i].hasOwnProperty('url') ||
        newslist[i]['url'] === "" ||
        !newslist[i].hasOwnProperty('title')||
        newslist[i]['title'] === "" ||
        !newslist[i].hasOwnProperty('abstract') ||
        newslist[i]['abstract'] === "" ||
        !newslist[i].hasOwnProperty('published_date') ||
        newslist[i]['published_date'] === "" ||
        !newslist[i].hasOwnProperty('multimedia')||
        newslist[i]['multimedia'] === null){
      continue;
    }
    let news = {};
    news.id = newslist[i]['url'];
    news.title = newslist[i]['title'];
    news.url = newslist[i]['url'];
    news.date = newslist[i]['published_date'].substring(0,10);
    news.description = newslist[i]['abstract'];
    news.section = newslist[i]['section'];

    const array = newslist[i].multimedia;
    let flag = true;
    for(let i=0; i<array.length; i++){
      if(array[i]["width"] >= 2000){
        news.image = array[i]["url"];
        flag = false;
      }
    }
    if(flag)
      news.image = nytimesImage;

    results.push(news);
    if(results.length >= 10)
      break;
  }
  return results;
}

app.get('/api/nytimes/section/home', (req, res) => {
  const url = NYTimes_Home_URL;
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const newslist = resp.results;
      res.send(JSON.stringify({ "results": getNYTimesSection(newslist)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


app.get('/api/nytimes/section/:Name', (req, res) => {
  const url = NYTimes_Section_URL(req.params.Name);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const newslist = resp.results;
      res.send(JSON.stringify({ "results": getNYTimesSection(newslist)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


function getNYTimesNews(data){
  if( !data.hasOwnProperty('headline') ||
      !data['headline'].hasOwnProperty('main') ||
      data['headline']['main'] === "" ||
      !data.hasOwnProperty('web_url')||
      data['web_url'] === "" ||
      !data.hasOwnProperty('abstract') ||
      data['abstract'] === "" ||
      !data.hasOwnProperty('pub_date') ||
      data['pub_date'] === "" ||
      !data.hasOwnProperty('section_name') ||
      data['section_name'] === "" ||
      !data.hasOwnProperty('multimedia')){
    return {};
  }

  let news = {};
  news.id = data['web_url'];
  news.title = data['headline']['main'];
  news.url = data['web_url'];
  news.date = data['pub_date'].substring(0,10);
  news.description = data['abstract'];
  news.source = "NYTIMES";
  news.section = data['section_name'].toLowerCase();

  const array = data['multimedia'];
  if(array === null){
    news.image = nytimesImage;
  }
  let flag = true;
  for(let i=0; i<array.length; i++){
    if(array[i]["width"] >= 2000){
      news.image = "https://nyt.com/" + array[i]["url"];
      flag = false;
    }
  }
  if(flag)
    news.image = nytimesImage;

  return news;
}


app.get('/api/nytimes/news/', (req, res) => {
  const url = NYTimes_News_URL(req.query.id);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      res.send(JSON.stringify({ "data": getNYTimesNews(resp.response.docs[0])}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


function getNYTimesQuery(newslist){
  let results = [];
  for(let i=0; i<newslist.length; i++){
    if( !newslist[i].hasOwnProperty('headline') ||
        !newslist[i]['headline'].hasOwnProperty('main') ||
        newslist[i]['headline']['main'] === "" ||
        !newslist[i].hasOwnProperty('web_url')||
        newslist[i]['web_url'] === "" ||
        !newslist[i].hasOwnProperty('abstract') ||
        newslist[i]['abstract'] === "" ||
        !newslist[i].hasOwnProperty('pub_date') ||
        newslist[i]['pub_date'] === "" ||
        !newslist[i].hasOwnProperty('news_desk') ||
        newslist[i]['news_desk'] === "" ||
        !newslist[i].hasOwnProperty('multimedia') ||
        newslist[i]['multimedia'] === null){
      continue;
    }
    let news = {};
    news.id = newslist[i]['web_url'];
    news.title = newslist[i]['headline']['main'];
    news.url = newslist[i]['web_url'];
    news.date = newslist[i]['pub_date'].substring(0,10);
    news.description = newslist[i]['abstract'];
    news.source = "NYTIMES";
    news.section = newslist[i]['news_desk'].toLowerCase();

    const array = newslist[i]['multimedia'];
    let flag = true;
    for(let i=0; i<array.length; i++){
      if(array[i]["width"] >= 2000){
        news.image = "https://nyt.com/" + array[i]["url"];
        flag = false;
      }
    }
    if(flag)
      news.image = nytimesImage;

    results.push(news);
  }
  return results;
}


app.get('/api/nytimes/query/', (req, res) => {
  const url = NYTimes_Query_URL(req.query.keyword);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      res.send(JSON.stringify({ "results": getNYTimesQuery(resp.response.docs)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


const port = 8000;
app.listen(port, () => console.log(`listening on port ${port}`));

module.exports = app;