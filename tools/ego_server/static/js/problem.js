var problemName = window.location.href.split("/").pop().replace(/#.*/, "");

$("#page-header").text(
    problemName + " problem. "
).append("<small>Statistics overview</small>");

function ajaxPerformanceRenderer(url, plot, options) {
    var ret = null;
    $.ajax({
        async: false,
        url: url,
        dataType:"json",
        success: function(data) {
         ret = data;
        }
    });
    var indexedRet = [[]];
    for (var vi=0; vi < ret.length; vi += 1) {
        indexedRet[0].push([vi, ret[vi]]);
    }
    return indexedRet;
};

var localCache = {
    /**
     * timeout for cache in millis
     * @type {number}
     */
    timeout: 30000,
    /** 
     * @type {{_: number, data: {}}}
     **/
    data: {},
    remove: function (url) {
        delete localCache.data[url];
    },
    exist: function (url) {
        return !!localCache.data[url] && ((new Date().getTime() - localCache.data[url]._) < localCache.timeout);
    },
    get: function (url) {
        console.log('Getting in cache for url' + url);
        return localCache.data[url].data;
    },
    set: function (url, cachedData, callback) {
        localCache.remove(url);
        localCache.data[url] = {
            _: new Date().getTime(),
            data: cachedData
        };
        if ($.isFunction(callback)) callback(cachedData);
    }
};

$.ajaxPrefilter(function (options, originalOptions, jqXHR) {
    if (options.cache) {
        var complete = originalOptions.complete || $.noop,
            url = originalOptions.url;
        //remove jQuery cache as we have our own localCache
        options.cache = false;
        options.beforeSend = function () {
            if (localCache.exist(url)) {
                complete(localCache.get(url));
                return false;
            }
            return true;
        };
        options.complete = function (data, textStatus) {
            localCache.set(url, data, complete);
        };
    }
});


function loadVariableSliceChart(chartId, variableName) {
    var lastAlert = $("#variable-slice-body div").last();
    if (lastAlert.hasClass("alert")) {
        lastAlert.remove();
    }
    // try {
        $("#" + chartId).removeClass("hidden");
        
        

        var sliceData = null;
        $.ajax({
            async: false,
            url: "/api/problem/"+problemName+ "/variable_slice?variable_name=" + variableName,
            dataType:"json",
            cache: true,
            complete: function(data) { 
                sliceData = JSON.parse(data.responseText);
            }
        }).fail(
            function(jqXHR, textStatus, errorThrown) {
                if (textStatus == "canceled") {
                    return;
                }
                
                $("#" + chartId).addClass("hidden");
                $("#variable-slice-body").append(
                    "<div class='alert alert-danger'><strong>Post request failed!</strong> " + jqXHR["responseText"] + "</div>"
                );
                console.log(textStatus);
                console.log(errorThrown);
            }
        );
        
        if (variableName in variableSlicePlots) {
            variableSlicePlots[variableName].destroy();
        }

        variableSlicePlots[variableName] = $.jqplot(chartId, [sliceData["mean"], sliceData["points"], sliceData["minimum"]], {
            axes: {
                xaxis: { tickOptions: { formatString:'%.3f' } },
                yaxis: { tickOptions: { formatString:'%.3f'} }
            },
            seriesColors: ['#17BDB8', '#EBC400', '#FF0000'],
            series: [
                { 
                    markerOptions: { size: 1, style: "circle" },
                    rendererOptions: { bandData: [sliceData["leftBand"], sliceData["rightBand"]] }
                }, 
                { showLine:false},
                { showLine:false}
            ],
            highlighter: {
                show: true,
                sizeAdjust: 7.5
            },
            cursor: {
                show: false
            }
        });

        // , 
        
    // } catch(err) {
    //     if (err.message == "No data specified") {
    //         $("#" + chartId).addClass("hidden");
    //         $("#variable-slice-body").append(
    //             "<div class='alert alert-danger'><strong>Can't build slice for empty model!</strong></div>"
    //         );
    //         console.log(err);    
    //     } else {
    //         throw err;
    //     }                    
    // } 
}
var variableSlicePlots = {}

$(document).ready(function(){
    var perfJsonUrl = "/api/problem/"+problemName+ "/performance";
    try {
        var problemPerformanceChart = $.jqplot('problem-performance-area-chart', perfJsonUrl, {
            dataRenderer: ajaxPerformanceRenderer,
            dataRendererOptions: {
              unusedOptionalUrl: perfJsonUrl
            },
            axes:{
                xaxis: { tickOptions: { formatString:'%d' } },
                yaxis: { tickOptions: { formatString:'%.2f'} }
            },
            highlighter: {
                show: true,
                sizeAdjust: 7.5
            },
            cursor: {
                show: false
            }
        });
        $("#problem-performance-panel-title").empty();
        $("#problem-performance-panel-title").append("<i class='fa fa-bar-chart-o fa-fw'></i> Problem performance");
               
    } catch(err) {
        if (err.message == "No data specified") {
            $("#problem-performance-panel-title").empty();
            $("#problem-performance-panel-title").append("<i class='fa fa-bar-chart-o fa-fw'></i> Problem performance (no data specified)");
            console.log(err);    
        } else {
            throw err;
        }
    }
    
    
    $.getJSON("/api/problem/" + problemName + "/specification", function(data) {
        $("#variable-slices-tab").empty();
        $("#variable-slices-tab-content").empty();
        
        $("#variable-slices-tab").append(
            "<li role='presentation' class='active'></li>"
        );
        $("#variable-slices-tab-content").append(
            "<div id='empty-tab' class='tab-pane in'></div>"
        );
        
        $.each(data["Variable"], function(key, val) {
            var tabHeadingIdPfx = "tab-heading-";
            var aHrefTabIdPfx = "link-to-tab-";
            var paneIdPfx = "variable-slice-pane-";
            var chartIdPfx = "variable-slice-chart-";

            $("#variable-slices-tab").append(
                "<li id='"+tabHeadingIdPfx+key+"' role='presentation'>" +
                    "<a data-toggle='tab' href='#"+val["Name"]+"' id='" + aHrefTabIdPfx+key + "'>" + val["Name"] + "</a>" + 
                "</li>"
            );

            $("#variable-slices-tab-content").append(
                "<div id='" + paneIdPfx+key +"' class='tab-pane in>" +
                    "<div class='col-md-12'>" + 
                        "<br>" +
                        "<div id='" + chartIdPfx+key + "'></div>" +
                    "</div>" + 
                "</div>"
            );

            $("#" + aHrefTabIdPfx+key).on("click", function() {
                $.each(data["Variable"], function(skey, sval) {
                    $("#"+paneIdPfx+skey).removeClass("active");
                    $("#"+tabHeadingIdPfx+skey).removeClass("active");
                });
                
                $("#"+paneIdPfx+key).addClass("active");
                $("#"+tabHeadingIdPfx+key).addClass("active");
                
                loadVariableSliceChart(chartIdPfx+key, val["Name"]);
                window.location.href = "#" + val["Name"];
            });

            var varName = window.location.href.split("#").pop();
            if (varName == val["Name"]) {
                $("#" + aHrefTabIdPfx+key).click();
            }
        });
    });

});


