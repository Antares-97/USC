import React, { Component } from 'react';
import axios from 'axios';
import TagBadge from './badge';
import ShareModal from './share';
import { withRouter } from 'react-router-dom';
import queryString from 'query-string';


class SearchResults extends Component {
    constructor(props){
        super(props);
        this.state = {
            query: '',
            results: [],
        };
    }

    componentWillMount(){
        this.props.handleShowSwitch(false);
        this.props.handleClick(false, 'results');
        const queryParams = queryString.parse(this.props.location.search);
        const keyword = queryParams.keyword;
        this.setState({
            query: keyword,
        });
        const url = 'https://cs571-hw8-2270250947.appspot.com/api/' + (this.props.isOn? 'guardian': 'nytimes') + '/query/?keyword=' + keyword;
        axios.get(url)
        .then(resp => {
            this.setState({results: resp.data.results}); 
        });
    }

    componentWillReceiveProps(nextProps){
        if(this.props.location.search !== nextProps.location.search){
            this.setState({query: ''});
            this.loadResults(nextProps.location.search);
        }
    }

    loadResults(key){
        const queryParams = queryString.parse(key);
        const keyword = queryParams.keyword;
        this.setState({
            query: keyword,
        });
        const url = 'https://cs571-hw8-2270250947.appspot.com/api/' + (this.props.isOn? 'guardian': 'nytimes') + '/query/?keyword=' + keyword;
        axios.get(url)
        .then(resp => {
            this.setState({results: resp.data.results}); 
        });
    }


    handleNewsClick = (id) => {
        let url = '/news?id=' + id;
        this.props.history.push(url);
    }

    customSubstr(str, br){
        if(br+7 >= str.length)
            return str;
        if(br < str.length && str.charAt(br) !== ' ')
            br = str.indexOf(' ', br-3);
        return str.substr(0, br) + "...";
    }


    render() { 
        const newslist = this.state.results;
        if(newslist.length===0){
            return(
                <div className="page">
                    <h5>Results</h5>
                </div>
            );
        }

        return(
            <div className="page">
                <h5>Results</h5>
                {newslist.map((news) => (
                    <div className="news" key={news.id} onClick={() => this.handleNewsClick(news.id)}>
                        <div className="title">
                            {this.customSubstr(news.title, 43)}
                            <ShareModal 
                                title={<div><h2>{news.source}</h2> <p>{news.title}</p></div>} 
                                url={news.url}
                            />
                        </div>

                        <div className="img">
                            <img src={news.image} alt="" />
                        </div>

                        <div className="text">
                            <h2>
                                {news.date}
                                <span className="tag"><TagBadge tag={news.section} style={{fontSize: "11px"}}/></span>
                            </h2>
                        </div>
                    </div>
                ))}
            </div>
        );
    }
}
 
export default withRouter(SearchResults);