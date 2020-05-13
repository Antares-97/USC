var createError = require('http-errors');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
const guardian_api_key = '2e294fb6-60a7-497d-a1c0-653035370a6d';
const Guardian_Home_URL = 'https://content.guardianapis.com/' +
    'search?orderby=newest&show-fields=starRating,headline,thumbnail,short-url' +
    '&api-key=' + guardian_api_key;
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


const express = require('express');
const app = express();
const XMLHttpRequest = require('xmlhttprequest').XMLHttpRequest;


app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "localhost:3000"); // update to match the domain you will make the request from
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});


function getGuardianHome(newslist){
  let results = [];
  for(let i=0; i<newslist.length; i++){
    if(!newslist[i].hasOwnProperty('id') ||
        newslist[i]['id'] === "" ||
        !newslist[i].hasOwnProperty('sectionName') ||
        newslist[i]['sectionName'] === "" ||
        !newslist[i].hasOwnProperty('webTitle')||
        newslist[i]['webTitle'] === "" ||
        !newslist[i].hasOwnProperty('webUrl')||
        newslist[i]['webUrl'] === "" ||
        !newslist[i].hasOwnProperty('webPublicationDate') ||
        newslist[i]['webPublicationDate'] === ""){
      continue;
    }
    let news = {};
    news.id = newslist[i]['id'];
    news.title = newslist[i]['webTitle'];
    news.url = newslist[i]['webUrl'];
    // news.date = newslist[i]['webPublicationDate'].substring(0,10);
    news.time = newslist[i]['webPublicationDate'];
    news.section = newslist[i]['sectionName'];

    if(!newslist[i].hasOwnProperty('fields') ||
        !newslist[i]['fields'].hasOwnProperty('thumbnail') ||
        newslist[i]['fields']['thumbnail'] === ""){
      news.image = guardianImage;
    }
    else{
      news.image = newslist[i]['fields']['thumbnail'];
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
      res.send(JSON.stringify({ "results": getGuardianHome(newslist)}));
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
        !newslist[i].hasOwnProperty('sectionName') ||
        newslist[i]['sectionName'] === "" ||
        !newslist[i].hasOwnProperty('webTitle')||
        newslist[i]['webTitle'] === "" ||
        !newslist[i].hasOwnProperty('webUrl')||
        newslist[i]['webUrl'] === "" ||
        !newslist[i].hasOwnProperty('webPublicationDate') ||
        newslist[i]['webPublicationDate'] === "" ||
        !newslist[i]['blocks'].hasOwnProperty('body') ||
        !newslist[i]['blocks'].hasOwnProperty('main') ||
        !newslist[i]['blocks']['main'].hasOwnProperty('elements')){
      continue;
    }
    let news = {};
    news.id = newslist[i]['id'];
    news.title = newslist[i]['webTitle'];
    news.url = newslist[i]['webUrl'];
    news.time = newslist[i]['webPublicationDate'];
    news.section = newslist[i]['sectionName'];

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
      !data.hasOwnProperty('sectionName') ||
      data['sectionName'] === "" ||
      !data.hasOwnProperty('webTitle')||
      data['webTitle'] === "" ||
      !data.hasOwnProperty('webUrl')||
      data['webUrl'] === "" ||
      !data.hasOwnProperty('webPublicationDate') ||
      data['webPublicationDate'] === "" ||
      !data['blocks'].hasOwnProperty('body') ||
      !data['blocks']['body'][0].hasOwnProperty('bodyHtml') ||
      data['blocks']['body'][0]['bodyHtml'] === "" ||
      !data['blocks'].hasOwnProperty('main') ||
      !data['blocks']['main'].hasOwnProperty('elements')){
    return {};
  }

  let news = {};
  news.id = data['id'];
  news.title = data['webTitle'];
  news.url = data['webUrl'];
  news.time = data['webPublicationDate'];
  news.section = data['sectionName'];

  var array = data['blocks']['body'];
  var description = ""
  for(let i=0; i<array.length; i++){
    description += array[i]['bodyHtml'].toString().replace(/(\r\n*|\n*|\r*)/gm,"");
  }
  news.description = description

  array = data['blocks']['main']['elements'];
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


// usage: /api/guardian/news/?id=...
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


function getGuardianSearch(newslist){
  let results = [];
  for(let i=0; i<newslist.length; i++){
    if(!newslist[i].hasOwnProperty('id') ||
        newslist[i]['id'] === "" ||
        !newslist[i].hasOwnProperty('sectionName') ||
        newslist[i]['sectionName'] === "" ||
        !newslist[i].hasOwnProperty('webTitle')||
        newslist[i]['webTitle'] === "" ||
        !newslist[i].hasOwnProperty('webUrl')||
        newslist[i]['webUrl'] === "" ||
        !newslist[i].hasOwnProperty('webPublicationDate') ||
        newslist[i]['webPublicationDate'] === "" ||
        !newslist[i]['blocks'].hasOwnProperty('main') ||
        !newslist[i]['blocks']['main'].hasOwnProperty('elements')){
      continue;
    }
    let news = {};
    news.id = newslist[i]['id'];
    news.title = newslist[i]['webTitle'];
    news.url = newslist[i]['webUrl'];
    news.time = newslist[i]['webPublicationDate'];
    news.section = newslist[i]['sectionName'];

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


// usage: /api/guardian/query/?keyword=...
app.get('/api/guardian/query/', (req, res) => {
  const url = Guardian_Query_URL(req.query.keyword);
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      const resp = JSON.parse(xhr.responseText);
      const newslist = resp.response.results;
      res.send(JSON.stringify({ "results": getGuardianSearch(newslist)}));
    }
  };
  xhr.open('GET', url, true);
  xhr.send();
});


const googleTrends = require('google-trends-api');
// usage: /api/trends/?keyword=...
app.get('/api/trends/', (req, res) =>{
  const keyword = req.query.keyword;
  googleTrends.interestOverTime({
    keyword: keyword,
    startTime: new Date('2019-06-01')
  }).then(function(results){
    const timelineData = JSON.parse(results)["default"]["timelineData"];
    let data = [];
    for(let i=0; i<timelineData.length; i++){
      data.push(timelineData[i]["value"][0]);
    }
    res.send(JSON.stringify({"data": data}));
  }).catch(function(err){
    console.error(err);
  });
});


const port = 8000;
app.listen(port, () => console.log(`listening on port ${port}`));

module.exports = app;
