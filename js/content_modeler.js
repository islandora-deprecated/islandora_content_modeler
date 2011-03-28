var collection = new Array('Root');
var model = false;
   
$(document).ready(function () { 
    
    $(document).ajaxStart(function(){
        $('#ajaxBusy').show();
    }).ajaxStop(function(){
        $('#ajaxBusy').hide();
    });
	
    $('#Notification').jnotifyInizialize({
        oneAtTime: false,
        appendType: 'append'
    })
    .css({
        'marginTop': '20px',
        'left': '20px',
        'width': '500px',
        'z-index': '9999'
    });
	
    updateCollectionTree();
    updateModelList();
      
    $('.refresh').click(function () {
        $('#Notification').jnotifyAddMessage({
            text: 'Refreshed collection '+collection[collection.length-1]+' and model '+(model==false?'list':model)+'.',
            permanent: false
        });
        updateCollectionTree();
        if (model == false)
            updateModelList();
        else
            updateModelTree();
        return false;
    });
      
    $('.list').click(function () {
        $('#Notification').jnotifyAddMessage({
            text: 'Refreshed model list.',
            permanent: false
        });
        model = false;
        updateModelList();
        return false;
    });
    
      
    function updateBreadcrumbs()
    {
        var content = '';
        for (var i = 0; i < collection.length; i++)
        {
            content += '<a href="#" class="collection_crumb" id="'+i+'">'+collection[i]+'</a> /';
        }
        $('#collection_crumbs').html(content);
    }

    function setFormCallbacks(handler, callback) {
        $(handler.module).change(function () {
            $.get('modeler/ajax/' + callback + '/', {
                module: $(handler.module).val()
            }, function (data) {
                $(handler.filename).html(data);
            });
            $(handler.classname).html('');
        });
        $(handler.filename).change(function () {
            $.get('modeler/ajax/getClasses/', {
                module: $(handler.module).val(),
                file: $(handler.filename).val(),
                className:  $(handler.classname).val()
            },
            function (data) {
                $(handler.classname).html(data);
            });
        });
    }

    function handleForm()
    {
        var document = {
            module: '#edit-form-document-module',
            filename: '#edit-form-document-filename',
            classname: '#edit-form-document-class'
        }
        var ingest = {
            module: '#edit-form-ingest-module',
            filename: '#edit-form-ingest-filename',
            classname: '#edit-form-ingest-class'
        }
        var edit = {
            module: '#edit-form-edit-module',
            filename: '#edit-form-edit-filename',
            classname: '#edit-form-edit-class'
        }
        setFormCallbacks(document, 'getDocuments');
        setFormCallbacks(ingest, 'getBuilders');
        setFormCallbacks(edit, 'getBuilders');

        $('.form-item #edit-form-filename').hide();
        $('.form-item #edit-form-filename').before('<select id="edit-form-filename-select"></select>');
        $('#edit-form-filename-select').change(function () {
            $('#edit-form-filename').val($('#edit-form-filename-select').val());
            $.get('modeler/ajax/getClasses/', {
                module: $('#edit-form-module').val(),
                file: $('#edit-form-filename').val(),
                className:  $('#edit-form-class').val()
            },
            function (data) {
                $('#edit-form-class-select').html(data);
            });
        });
	      
	      
        $('.form-item #edit-form-class').hide();
        $('.form-item #edit-form-class').before('<select id="edit-form-class-select"></select>');
        $('#edit-form-class-select').change(function () {
            $('#edit-form-class').val($('#edit-form-class-select').val());
            $.get('modeler/ajax/getMethods/', {
                module: $('#edit-form-module').val(),
                file: $('#edit-form-filename').val(),
                className:  $('#edit-form-class').val()
            },
            function (data) {
                $('#edit-form-method-select').html(data);
                $('#edit-form-handler-select').html(data);
            });
        });

        if ($('#edit-form-file').val() || $('#edit-form-class').val() || $('#edit-form-filename').val())
        {
            if($('#edit-form-module').hasClass('form-builder'))
            {
                $.get('modeler/ajax/getForms/', {
                    module: $('#edit-form-module').val(),
                    file: $('#edit-form-filename').val()
                }, function (data) {
                    $('#edit-form-filename-select').html(data);
                });
            }
            else
            {
                $.get('modeler/ajax/getFiles/', {
                    module: $('#edit-form-module').val(),
                    file: $('#edit-form-filename').val()
                }, function (data) {
                    $('#edit-form-filename-select').html(data);
                });
            }

            $.get('modeler/ajax/getClasses/', {
                module: $('#edit-form-module').val(),
                file: $('#edit-form-filename').val(),
                className: $('#edit-form-class').val()
            }, function (data) {
                $('#edit-form-class-select').html(data);
            });
        }
	      
        $('.form-item #edit-form-method, .form-item #edit-form-handler').hide();
        $('.form-item #edit-form-method').before('<select id="edit-form-method-select"></select>');
        $('.form-item #edit-form-handler').before('<select id="edit-form-handler-select"></select>');
        $('#edit-form-method-select').change(function () {
            $('#edit-form-method').val($('#edit-form-method-select').val());
        });
        $('#edit-form-handler-select').change(function () {
            $('#edit-form-handler').val($('#edit-form-handler-select').val());
        });

	      
        if ( $('#edit-form-class').val())
        {
            $.get('modeler/ajax/getMethods/', {
                module: $('#edit-form-module').val(),
                file: $('#edit-form-filename').val(),
                className: $('#edit-form-class').val(),
                method: $('#edit-form-method').val()
            }, function (data) {
                $('#edit-form-method-select').html(data);
            });
            $.get('modeler/ajax/getMethods/', {
                module: $('#edit-form-module').val(),
                file: $('#edit-form-filename').val(),
                className: $('#edit-form-class').val(),
                method: $('#edit-form-handler').val()
            }, function (data) {
                $('#edit-form-handler-select').html(data);
            });
        }
	      
        $('#ajaxForm #cancel').click(function (d) {
            $('#ajaxForm').fadeOut();
            return false;
        });
    	 
        $('#ajaxForm form').submit(function (d) {
            $.post(d.target.action,
                $('#'+d.target.id).serialize()+'&op=Save',
                function (data)
                {
                    lines = data.split(':');
                    if (lines.shift() == 'success')
                    {
                        $('#Notification').jnotifyAddMessage({
                            text: lines.join(':'),
                            permanent: false
                        });
                        $('#ajaxForm').fadeOut();
                        if (model == false)
                            updateModelList();
                        else
                            updateModelTree();
                        updateCollectionTree();
                    }
                    else
                    {
                        $('#ajaxForm').html(data);
                        handleForm();
                    }
                });
            return false;
        });
    }

    function buttonIcons()
    {
        $('.ajaxButtonIcon').unbind();
        $('.ajaxButtonIcon').click(function () {
            $.get('modeler/ajax/button', {
                formReq: this.id
            }, function (data) {
                lines = data.split(':');
                if (lines.shift() == 'success')
                {
                    $('#Notification').jnotifyAddMessage({
                        text: lines.join(':'),
                        permanent: false
                    });
                    if (model == false)
                        updateModelList();
                    else
                        updateModelTree();
                    updateCollectionTree();
                } else
{
                    $('#Notification').jnotifyAddMessage({
                        text: data,
                        permanent: false,
                        type: 'error'
                    });
                }
            });
		    	
        });
    }
	  
    function formIcons()
    {
        $('.ajaxFormIcon').unbind();
        $('.ajaxFormIcon').click(function () {
            var params=this.id.split(' ');
            var formName=params.shift();
            $.get('modeler/ajax/processForm/'+formName, {
                formReq: params.join(' ')
            }, function (data) {
                if (data == '')
                {
                    $('#Notification').jnotifyAddMessage({
                        text: 'Error: Unable to load requested form <b>\''+formName+'\'</b>.',
                        permanent: false,
                        type: 'error'
                    });
                } else
{
                    $('#ajaxForm').html(data).fadeIn();
                    handleForm();
                }
            });
		    	
            return false;
        });
    }

    function updateModelList() {
        $.get('modeler/ajax/listModels' , function (j) {
            $('#model_tree').html(j);
            $('#model_tree ul').treeview({
                animated: "fast",
                collapsed: true,
                unique: false,
                persist: "cookie",
                cookieId: "modelTree"
            });
		    
            $(".list_model").click( function () {
                model = this.id;
                updateModelTree();
                $('#Notification').jnotifyAddMessage({
                    text: 'Displayed model '+this.id,
                    permanent: false
                });
                return false;
            });
		    
            buttonIcons();
            formIcons();
		    
        });
    }
	  
    function updateModelTree() {
        $.get('modeler/ajax/model', {
            model_pid: model
        } , function (j) {
            $('#model_tree').html(j);
            $('#model_tree ul').treeview({
                animated: "fast",
                collapsed: true,
                unique: false,
                persist: "cookie",
                cookieId: "modelTree"
            });
		    
            buttonIcons();
            formIcons();
		    
        });
    }
      
    function updateCollectionTree() {
        collection_pid = collection[collection.length-1];
        if (collection_pid == 'Root')
            collection_pid = false;

        $.get('modeler/ajax/collection', {
            collection_pid: collection_pid
        } , function (j) {
            updateBreadcrumbs();
			
            $('#collection_tree').html(j);
            $('#collection_tree ul').treeview({
                animated: "fast",
                collapsed: true,
                unique: false,
                persist: "cookie",
                cookieId: "collectionTree"
            });
	
            buttonIcons();
            formIcons();
			
            $(".collection_model").click( function () {
                model = this.id;
                updateModelTree();
                $('#Notification').jnotifyAddMessage({
                    text: 'Displayed model '+this.id,
                    permanent: false
                });
                return false;
            });
	
            $(".collection_child").click( function () {
                collection.push(this.id);
                updateCollectionTree();
                $('#Notification').jnotifyAddMessage({
                    text: 'Switched to collection '+this.id,
                    permanent: false
                });
                return false;
            });
		    
            $(".collection_crumb").click( function () {
                var pop_no = collection.length-this.id-1;
                for (var i =0; i < pop_no; i++)
                    collection.pop();
                updateCollectionTree();
                $('#Notification').jnotifyAddMessage({
                    text: 'Switched to collection '+collection[collection.length-1],
                    permanent: false
                });
                return false;
            });
        });
    }
});
