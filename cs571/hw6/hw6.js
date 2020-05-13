var myWords=null, cnn=null, fox=null, topnews=null;

function loadjson() {
    var xhr = new XMLHttpRequest();
    var url = '/headlines';
    xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200) {
            var data = JSON.parse(xhr.responseText);
            myWords = data.wordCloud;
            cnn = data.cnn;
            fox = data.fox;
            topnews = data.top;
            makeSlides();
            showSlides();
            drawcloud();
            cnnfox();
        }
    }
    xhr.open('GET', url, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send();
}

function makeSlides(){
    var slidewindow = document.getElementById('slideshow-container');
    for(var i=0; i<topnews.length; i++)
    {
        var slide = document.createElement('div');
        slide.setAttribute('class', 'mySlides');
        slide.setAttribute('onclick', 'window.open("'+topnews[i]['url']+'");');
        var pic = document.createElement('img');
        pic.setAttribute('src', topnews[i]['urlToImage']);
        slide.appendChild(pic);
        var text = document.createElement('div');
        text.setAttribute('class', 'text');
        var title = document.createElement('p');
        title.innerHTML = '<b>' + topnews[i]['title'] + '</b><br>';
        text.appendChild(title);
        var desc = document.createElement('p');
        desc.setAttribute('style', 'font-size:14px;');
        desc.innerHTML = topnews[i]['description'];
        text.appendChild(desc);
        slide.appendChild(text);
        slidewindow.appendChild(slide);
    }
}

function showSlides(){
    var slideIndex = 0;
    moveSlides();
    function moveSlides() {
        var slides = document.getElementsByClassName('mySlides');
        for (var i = 0; i < slides.length; i++) {
            slides[i].style.display = "none";
        }
        slideIndex++;
        if (slideIndex > slides.length) {slideIndex = 1}
        slides[slideIndex-1].style.display = "block";
        setTimeout(moveSlides, 3000);
    }
}


function cnnfox(){
    function makeNews(name){
        var news = null;
        if(name === 'cnn') news = cnn;
        else if(name === 'fox') news = fox;
        if(news == null) return;

        var table = document.getElementById(name);
        var row = document.createElement('tr');
        for(var i=0; i<news.length; i++){
            var cell = document.createElement('td');
            cell.setAttribute('class', 'cell');
            cell.setAttribute('onclick', 'window.open("'+news[i]['url']+'");');
            var divimg = document.createElement('div');
            divimg.setAttribute('class', 'img');
            var divtext = document.createElement('div');
            divtext.setAttribute('class', 'text');

            var pic = document.createElement('img');
            pic.setAttribute('src', news[i]['urlToImage']);
            divimg.appendChild(pic);

            var title = document.createElement('p');
            title.innerHTML = '<b>' + news[i]['title'] + '</b><br>';
            var desc = document.createElement('p');
            desc.setAttribute('style', 'font-size:12px;');
            desc.innerHTML = news[i]['description'];

            divtext.appendChild(title);
            divtext.appendChild(desc);
            cell.appendChild(divimg);
            cell.appendChild(divtext);
            row.appendChild(cell);
        }
        table.appendChild(row);
    }
    makeNews('cnn');
    makeNews('fox');
}


function switchpage(str){
    var clickable = document.getElementById('link');
    if(clickable.textContent !== str) return;

    var current = document.getElementById('selected');
    clickable.setAttribute('id', 'selected');
    current.setAttribute('id', 'link');

    var googlenews = document.getElementById('googlenews');
    var search = document.getElementById('search');
    if(str === 'Search'){
        googlenews.style.display = 'none';
        search.style.display = 'block';
        getsource('all');
        defaultDate();
    }
    else if(str === 'Google News'){
        googlenews.style.display = 'block';
        search.style.display = 'none';
    }
}


var sources = null;
function getsource(Category){
    var xhr = new XMLHttpRequest();
    var url = '/sources/' + Category;
    xhr.open('GET', url, true);
    xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200) {
            sources = JSON.parse(xhr.responseText).sources;
            var newssource = document.getElementById("Source");
            while (newssource.hasChildNodes()) {
                newssource.removeChild(newssource.firstChild);
            }
            var x = document.createElement("OPTION");
            x.text = 'all';
            x.value = 'all';
            x.setAttribute('selected', true);
            newssource.appendChild(x);
            for (var i = 0; i < 10; i++) {
                x = document.createElement("OPTION");
                x.text = sources[i]['name'];
                x.value = sources[i]['id'];
                newssource.appendChild(x);
            }
        }
    }
    xhr.send();
}


function changesource(){
    var Category = document.getElementById("Category").value;
    getsource(Category);
}


function dateString(Date){
    var date = Date.getDate();
    var month = Date.getMonth() + 1;
    var year = Date.getFullYear();
    var today = year + '-';
    if(date<10){
        date = '0' + date;
    }
    if(month<10){
        month = '0' + month;
    }
    today += month + '-' + date;
    return today;
}


function defaultDate(){
    var d = new Date();
    document.getElementById("To").defaultValue = dateString(d);
    d = new Date();
    d.setDate(d.getDate()-7);
    document.getElementById("From").defaultValue = dateString(d);
}


function reset(){
    document.getElementById("searchForm").reset();
    defaultDate();
    document.getElementById("all").selected = true;
}


function dateValidation(){
    var from = document.getElementById('From').value;
    var to = document.getElementById('To').value;
    var fromDate = new Date(from);
    var toDate = new Date(to);
    fromDate.setHours(0,0,0,0);
    toDate.setHours(0,0,0,0);
    if(fromDate.getTime() > toDate.getTime()){
        alert('Incorrect Time');
        return false;
    }
    return true;            // valid dates
}

var result = null;
function search(){
    if(!dateValidation()){
        return;
    }
    var url = '/search/';
    var x = document.getElementById('Keyword');
    url += x.value + '/';
    x = document.getElementById('Source');
    url += x.value + '/';
    x = document.getElementById('From');
    url += x.value + '/';
    x = document.getElementById('To');
    url += x.value;

    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200) {
            var response = JSON.parse(xhr.responseText);
            result = response.result;
            status = response.status;
            if(status==="ok"){
                displaySearch();
            }
            else if(status==="error"){
                var message = JSON.parse(result).message;
                alert(message);
            }
        }
    }
    xhr.send();
}


function displaySearch(){
    if(result === null){
        return;
    }
    if(result.length === 0){
        var x = document.getElementById('search_result');
        x.style.display = 'none';
        x = document.getElementById('result_btn');
        x.style.display = 'none';
        x = document.getElementById('alert');
        x.style.display = 'block';
    }
    else{
        var alarm = document.getElementById('alert');
        alarm.style.display = 'none';
        var btn = document.getElementById('result_btn');
        btn.style.display = result.length > 5 ? "block" : "none";

        var table = document.getElementById('search_result');
        table.style.display = 'block';
        while (table.hasChildNodes()) {
            table.removeChild(table.firstChild);
        }
        for(var i=0; i<result.length; i++){
            var row = document.createElement('tr');
            var cell = document.createElement('td');
            var card = document.createElement('div');
            card.setAttribute('class', 'card');
            card.setAttribute('onclick', 'this.setAttribute("class","detail");' +
                                                             'cardDetail('+i+');');

            var divimg = document.createElement('div');
            divimg.setAttribute('class', 'pic');
            var divtext = document.createElement('div');
            divtext.setAttribute('class', 'content');
            var divbtn = document.createElement('span');
            divbtn.setAttribute('class', 'close');
            divbtn.setAttribute('style', 'display:none');

            var pic = document.createElement('img');
            pic.setAttribute('src', result[i]['urlToImage']);
            divimg.appendChild(pic);

            var title = document.createElement('p');
            title.setAttribute('style', 'font-size:16px;font-family:Georgia;');
            title.innerHTML = '<b>' + result[i]['title'] + '</b>';
            var desc = document.createElement('p');
            desc.innerHTML = result[i]['description'] + '<br>';
            desc.setAttribute('style','display:none;font-family:Georgia');

            var shortdesc = document.createElement('p');
            shortdesc.innerHTML = customSubstr(result[i]['description'], 67) + '<br>';
            // shortdesc.setAttribute('class','nowrap');

            var author = document.createElement('p');
            author.setAttribute('style', 'display:none;font-family:Georgia;');
            author.innerHTML = '<b>Author: </b>' + result[i]['author'];
            var source = document.createElement('p');
            source.setAttribute('style', 'display:none;font-family:Georgia;');
            source.innerHTML = '<b>Source: </b>' + result[i]['source']['name'];
            var date = document.createElement('p');
            date.setAttribute('style', 'display:none;font-family:Georgia;');
            date.innerHTML = '<b>Date: </b>' + result[i]['publishedAt'].substring(5,7)+'/' +
                result[i]['publishedAt'].substring(8,10)+'/' + result[i]['publishedAt'].substring(0,4);

            var link = document.createElement('a');
            link.setAttribute('href', result[i]['url']);
            link.setAttribute('target', '_blank');
            link.setAttribute('style', 'display:none;font-family:Georgia;');
            link.innerHTML = 'See Original Post';

            divtext.appendChild(title);
            divtext.appendChild(author);
            divtext.appendChild(source);
            divtext.appendChild(date);
            divtext.appendChild(desc);
            divtext.appendChild(link);
            divtext.appendChild(shortdesc);

            divbtn.innerHTML = '&times;';

            card.appendChild(divimg);
            card.appendChild(divtext);
            card.appendChild(divbtn);
            cell.appendChild(card);
            row.appendChild(cell);
            if(i>4) row.style.display = 'none';
            table.appendChild(row);
        }

        var closebtns = document.getElementsByClassName("close");
        for (var i = 0; i < closebtns.length; i++) {
            closebtns[i].addEventListener("click", function() {
                event.stopPropagation();
                this.style.display = 'none';
                father = this.parentElement;
                father.className = 'card';
                father.style.cursor = 'pointer';
                var btn = father.children[2];
                btn.style.display = 'none';
                var text = father.children[1];
                text.children[1].style.display = 'none';
                text.children[2].style.display = 'none';
                text.children[3].style.display = 'none';
                text.children[4].style.display = 'none';
                text.children[5].style.display = 'none';
                text.children[6].style.display = 'block';
            });
        }
    }
}


function customSubstr(str, br){
    if(br < str.length && str.charAt(br) != ' ')
        br = str.indexOf(' ', br);
    return str.substr(0, br) + "...";
}


function cardDetail(i){
    var x = document.getElementById('search_result');
    cell = x.rows[i].firstChild;
    card = cell.firstChild;
    card.style.cursor = 'default';
    var btn = card.children[2];
    btn.style.display = 'block';
    var text = card.children[1];
    text.children[1].style.display = 'block';
    text.children[2].style.display = 'block';
    text.children[3].style.display = 'block';
    text.children[4].style.display = 'block';
    text.children[5].style.display = 'block';
    text.children[6].style.display = 'none';
}


var More = true;        // true - show more; false - show less
function showMoreLess(){
    if(More){
        var btn = document.getElementById('result_btn');
        btn.innerHTML = "Show Less";
        var table = document.getElementById('search_result');
        for(var i=5; i<table.rows.length; i++){
            table.rows[i].style.display = 'block';
        }
    }
    else{
        var btn = document.getElementById('result_btn');
        btn.innerHTML = "Show More";
        var table = document.getElementById('search_result');
        for(var i=5; i<table.rows.length; i++){
            table.rows[i].style.display = 'none';
        }
    }
    More = !More;
}
