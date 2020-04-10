import React, { Component } from 'react';
import {BrowserRouter, Route, Switch, Link} from 'react-router-dom';
import { MdBookmark, MdBookmarkBorder } from 'react-icons/md';
import Nav from 'react-bootstrap/Nav';
import NavBar from "./Components/navbar";
import Home from './Components/home.jsx';
import World from './Components/world.jsx';
import Politics from './Components/politics.jsx';
import Business from './Components/business.jsx';
import Tech from './Components/tech.jsx';
import Sports from './Components/sports.jsx';
import Bookmark from './Components/bookmark.jsx';
import SwitchToggle from './Components/switch';
import Article from './Components/article';
import Result from './Components/results';
import SearchBar from './Components/searchbar';
import './App.css';

const navlinks = [
    {"id": "Home", "url": "/"},
    {"id": "World", "url": "/world"},
    {"id": "Politics", "url": "/politics"},
    {"id": "Business", "url": "/business"},
    {"id": "Technology", "url": "/tech"},
    {"id": "Sports", "url": "/sports"},
];

class App extends Component {
    constructor(props){
        super(props);
        this.state = {
            isOn: window.localStorage.getItem('source')==="guardian"? true: false,
            source: window.localStorage.getItem('source') || 'guardian',
            activeLink: 'Home',
            showSwitch: true,
            isFavorite: false,
            clearQuery: null,
            folded: true,
        }
        this.handleSwitch.bind(this);
        this.handleClick.bind(this);
        this.handleShowSwitch.bind(this);
        this.handleBookMark.bind(this);
        this.handleClear.bind(this);
    }

    handleSwitch= () => {
        this.setState({
            isOn: !this.state.isOn,
            source: this.state.isOn===true? 'nytimes': 'guardian'
        });
    }

    handleClick = (flag, id) => {
        this.setState({
            isFavorite: flag,
            activeLink: id
        });
    }

    handleShowSwitch = (flag) => {
        this.setState({
            showSwitch: flag
        });
    }

    handleBookMark = (flag, news) => {
        if (typeof(news) !== "object" || news === {}) {
            return;
        }
        let favorites = window.localStorage.getItem('favorites');
        if (favorites === null){
            favorites = [];
        }
        else{
            favorites = JSON.parse(favorites);
        }
        let index = -1;
        for(let i=0; i<favorites.length; i++){
            if(favorites[i].id===news.id){
                index = i;
                break;
            }
        }
        if((flag === true && index === -1) || (flag === false && index !== -1)){
            return;
        }
        if(flag){
            favorites.splice(index, 1);
            window.localStorage.setItem(news.id+'-marked', "false");
        }
        else{
            favorites[favorites.length]=news;        //true: delete, false: add
        }
        window.localStorage.setItem('favorites', JSON.stringify(favorites));
    }

    handleClear = (f) => {
        this.setState({
            clearQuery: f,
        });
    }

    handleFolded = () => {
        this.setState({
            folded: !this.state.folded
        })
    }

    render() {
        window.localStorage.setItem('source', this.state.source);
        return (
            <BrowserRouter>
                <NavBar searchBar={<SearchBar handleClear={this.handleClear}/>}>
                    <Nav>
                        {navlinks.map((link) => (
                            <Nav.Link key={link.id}>
                            <Link
                             to={link.url}
                             className={link.id === this.state.activeLink? "react-navbar-active-link": "react-navbar-default-link"}
                             onClick={() => this.handleClick(false, link.id)}>
                                {link.id}
                            </Link>
                            </Nav.Link>
                            
                        ))}
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        <Nav.Link>
                            <Link to="/bookmark" className="react-navbar-bookmark">
                                {this.state.isFavorite? <MdBookmark />: <MdBookmarkBorder />}
                            </Link>
                        </Nav.Link>
                        <Nav.Link><SwitchToggle isOn={this.state.isOn} onClick={this.handleSwitch} style={this.state.showSwitch?{display: "block"}: {display: "none"} }/></Nav.Link>
                    </Nav>
                </NavBar>
    
                <Switch>
                    <Route exact path="/" render={(props) => <Home {...props} isOn={this.state.isOn} handleShowSwitch={this.handleShowSwitch} handleClear={this.state.clearQuery}/>}/>
                    <Route exact path="/world" render={(props) => <World {...props} isOn={this.state.isOn} handleShowSwitch={this.handleShowSwitch} handleClear={this.state.clearQuery}/>}/>
                    <Route exact path="/politics" render={(props) => <Politics {...props} isOn={this.state.isOn} handleShowSwitch={this.handleShowSwitch} handleClear={this.state.clearQuery}/>}/>
                    <Route exact path="/business" render={(props) => <Business {...props} isOn={this.state.isOn} handleShowSwitch={this.handleShowSwitch} handleClear={this.state.clearQuery}/>}/>
                    <Route exact path="/tech" render={(props) => <Tech {...props} isOn={this.state.isOn} handleShowSwitch={this.handleShowSwitch} handleClear={this.state.clearQuery}/>}/>
                    <Route exact path="/sports" render={(props) => <Sports {...props} isOn={this.state.isOn} handleShowSwitch={this.handleShowSwitch} handleClear={this.state.clearQuery}/>}/>
                    <Route exact path="/bookmark" render={(props) => 
                                                            <Bookmark {...props} 
                                                            isOn={this.state.isOn} 
                                                            handleShowSwitch={this.handleShowSwitch}
                                                            handleBookMark={this.handleBookMark}
                                                            handleClick={this.handleClick}
                                                            handleClear={this.state.clearQuery}
                                                            />
                                                        }/>
                    <Route path="/news" render={(props) => 
                                                <Article {...props} 
                                                handleShowSwitch={this.handleShowSwitch} 
                                                handleBookMark={this.handleBookMark}
                                                handleClick={this.handleClick}
                                                handleClear={this.state.clearQuery}
                                                />
                                            }/>
                    <Route path="/results" render={(props) =>
                                                <Result {...props} 
                                                isOn={this.state.isOn} 
                                                handleShowSwitch={this.handleShowSwitch}
                                                handleClick={this.handleClick}
                                                handleClear={this.state.clearQuery}
                                                />
                                            }/>
                </Switch>
            </BrowserRouter>
        );
    }
}
 
export default App;