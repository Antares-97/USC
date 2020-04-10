import React, { Component } from 'react';
import axios from 'axios';
import LoadingPage from './loader';
import TagBadge from './badge';
import ShareModal from './share';
import { withRouter } from 'react-router-dom';
import '../App.css';


class homepage extends Component {
    constructor(props){
        super(props);
        this.state = {
            results: [],
        };
        this.handleCardClick.bind(this);
    }


    componentDidMount(){
        this.props.handleShowSwitch(true);
        if(this.props.handleClear!==null)
            this.props.handleClear();
        const url = 'https://cs571-hw8-2270250947.appspot.com/api/' + (this.props.isOn? 'guardian': 'nytimes') + '/section/home';
        axios.get(url)
        .then(resp => {
            this.setState({results: resp.data.results}); 
        })
    }

    componentWillReceiveProps(nextProps){
        if(this.props.isOn !== nextProps.isOn){
            this.setState({results: []});
            this.loadTopNews(nextProps.isOn);
        }
    }

    loadTopNews(source){
        const url = 'https://cs571-hw8-2270250947.appspot.com/api/' + (source? 'guardian': 'nytimes') + '/section/home';
        axios.get(url)
        .then(resp => {
            this.setState({results: resp.data.results}); 
        })
    }

    handleCardClick = (id) => {
        let url = '/news?id=' + id;
        this.props.history.push(url);
    }

    render() { 
        const newslist = this.state.results;
        if(newslist.length!==0){
            return (
                <div className="page">
                {newslist.map((news) => (
                    <div className="card" key={news['id']} onClick={() => this.handleCardClick(news['id'])}>
                        <div className="img">
                            <img src={news['image']} alt=""></img>
                        </div>
                        
                        <div className="text">
                            <h1>
                                {news['title']}
                                <ShareModal 
                                    title={news['title']} 
                                    url={news['url']}
                                />
                            </h1>
                            <div className="abstract">
                                {news['description']}
                            </div>
                            <br/>
                            <h2>
                                {news['date']}
                                <span className="tag"><TagBadge tag={news['section']} /></span>
                            </h2>
                            <div style={{clear: "both"}}></div>
                        </div>
                    </div>
                ))}
                </div>
            );
        }

        return(
            <div className="page">
                <LoadingPage />
            </div>
        );
    }
}
 
export default withRouter(homepage);
